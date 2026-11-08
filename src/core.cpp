#include "cheats.hpp"
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/Notification.hpp>
#include <chrono>
#include <deque>
#include <ctime>

static std::deque<double> g_clicks;
static int g_buffer = 0;
static bool g_injecting = false;
static int g_sessionDeaths = 0;
static int g_jumps = 0;

static double nowSec() {
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}

bool cheating() {
    for (auto id : {"speed_on", "noclip", "destroy_on", "autopilot", "autoclick", "autohold", "input_buffer"})
        if (on(id)) return true;
    return false;
}

static void applyFps(bool enabled) {
    double fps = enabled ? std::max(1.f, val("fps_val")) : 60.0;
    CCDirector::get()->setAnimationInterval(1.0 / fps);
}
void onCheatChanged(std::string const& id) {
    if (id.rfind("fps", 0) == 0) applyFps(on("fps_on"));
}

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        if (on("fps_on")) applyFps(true);
        return true;
    }
};

// ---- Speedhack ----
class $modify(CCScheduler) {
    void update(float dt) {
        if (on("speed_on")) dt *= val("speed_val");
        CCScheduler::update(dt);
    }
};

// ---- Click tracking + inputs buffer ----
class $modify(GJBaseGameLayer) {
    void handleButton(bool down, int button, bool p1) {
        if (down && !g_injecting) {
            g_clicks.push_back(nowSec());
            g_jumps++;
            if (on("input_buffer") && m_player1 && !m_player1->m_isOnGround)
                g_buffer = static_cast<int>(val("input_buffer_frames"));
        }
        GJBaseGameLayer::handleButton(down, button, p1);
    }
};

class $modify(MyPL, PlayLayer) {
    struct Fields {
        int frames = 0, badFrames = 0, deaths = 0;
        bool hit = false, held = false;
        int hold = 0;
        float time = 0.f, cpTimer = 0.f, clickTimer = 0.f, fps = 60.f;
        std::vector<std::pair<GameObject*, GameObjectType>> broken;
        CCLabelBMFont* label = nullptr;
    };

    bool init(GJGameLevel* lvl, bool replay, bool dontCreate) {
        if (!PlayLayer::init(lvl, replay, dontCreate)) return false;
        auto l = CCLabelBMFont::create("", "bigFont.fnt");
        this->addChild(l, 1000);
        m_fields->label = l;
        if (on("force_practice")) this->togglePracticeMode(true);
        return true;
    }

    void resetLevel() {
        auto f = m_fields.self();
        for (auto& [o, t] : f->broken) o->m_objectType = t;
        f->broken.clear();
        f->frames = f->badFrames = f->deaths = 0;
        f->hit = f->held = false;
        f->hold = 0; f->time = 0.f;
        g_buffer = 0;
        g_jumps = 0;
        PlayLayer::resetLevel();
    }

    void press(bool down) {
        g_injecting = true;
        this->handleButton(down, 1, true);
        g_injecting = false;
    }
    void tap(int frames) { press(true); m_fields->hold = frames; }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        auto f = m_fields.self();
        if (!m_player1) return;
        f->frames++;
        f->time += dt;
        float real = CCDirector::get()->getDeltaTime();
        if (real > 0.f) f->fps = f->fps * 0.95f + (1.f / real) * 0.05f;
        if (f->hit) { f->badFrames++; f->hit = false; }
        float acc = 100.f * (1.f - static_cast<float>(f->badFrames) / std::max(1, f->frames));

        m_player1->setVisible(!on("hide_p1"));
        if (m_player2) m_player2->setVisible(!on("hide_p2"));

        // --- input logic (one action per frame) ---
        if (f->hold > 0) {
            if (--f->hold == 0) press(false);
        } else {
            if (on("autohold") != f->held) { press(on("autohold")); f->held = on("autohold"); }
            else if (g_buffer > 0) {
                g_buffer--;
                if (m_player1->m_isOnGround) { g_buffer = 0; tap(2); }
            }
            else if (on("autoclick")) {
                f->clickTimer += real;
                if (f->clickTimer >= 1.f / std::max(1.f, val("autoclick_cps"))) { f->clickTimer = 0.f; tap(1); }
            }
            else if (on("autopilot") && m_player1->m_isOnGround && !m_player1->m_isDead) {
                float px = m_player1->getPositionX(), py = m_player1->getPositionY();
                float look = val("autopilot_look");
                for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
                    if (obj->m_objectType != GameObjectType::Hazard) continue;
                    float dx = obj->getPositionX() - px;
                    if (dx < look - 8.f || dx > look) continue;
                    if (std::abs(obj->getPositionY() - py) > 40.f) continue;
                    tap(static_cast<int>(val("autopilot_hold")));
                    break;
                }
            }
        }

        // --- auto checkpoints ---
        if (m_isPracticeMode && on("auto_checkpoint")) {
            f->cpTimer += dt;
            if (f->cpTimer >= val("checkpoint_secs")) { f->cpTimer = 0.f; this->createCheckpoint(); }
        }

        // --- labels ---
        if (!f->label) return;
        while (!g_clicks.empty() && g_clicks.front() < nowSec() - 1.0) g_clicks.pop_front();
        std::string s;
        if (on("lbl_cheat") && cheating()) s += "CHEATS ON\n";
        if (on("noclip") && on("noclip_label"))
            s += fmt::format("Noclip Accuracy: {:.2f}%\nNoclip Deaths: {}\n", acc, f->deaths);
        if (on("lbl_fps")) s += fmt::format("FPS: {:.0f}\n", f->fps);
        if (on("lbl_cps")) s += fmt::format("CPS: {}\n", g_clicks.size());
        if (on("lbl_frames")) s += fmt::format("Frame: {}\n", f->frames);
        if (on("lbl_time")) s += fmt::format("Time: {:.2f}s\n", f->time);
        if (on("lbl_percent")) s += fmt::format("Percent: {:.1f}%\n", this->getCurrentPercent());
        if (on("lbl_attempt")) s += fmt::format("Attempt: {}\n", m_attempts);
        if (on("lbl_best")) s += fmt::format("Best: {}%\n", m_level->m_normalPercent.value());
        if (on("lbl_level")) s += fmt::format("Level: {}\n", std::string(m_level->m_levelName));
        if (on("lbl_pos")) s += fmt::format("X: {:.0f} Y: {:.0f}\n", m_player1->getPositionX(), m_player1->getPositionY());
        if (on("lbl_speed")) s += fmt::format("Speed: x{:.2f}\n", on("speed_on") ? val("speed_val") : 1.f);
        if (on("lbl_session")) s += fmt::format("Session deaths: {}\n", g_sessionDeaths);
        if (on("lbl_objects")) s += fmt::format("Objects: {}\n", m_objects ? static_cast<int>(m_objects->count()) : 0);
        if (on("lbl_jumps")) s += fmt::format("Jumps: {}\n", g_jumps);
        if (on("lbl_pspeed")) s += fmt::format("Player speed: {:.2f}\n", m_player1->m_playerSpeed);
        if (on("lbl_gravity")) s += fmt::format("Gravity: {}\n", m_player1->m_isUpsideDown ? "flipped" : "normal");
        if (on("lbl_mode")) {
            auto* pl = m_player1;
            char const* mode = pl->m_isShip ? "Ship" : pl->m_isBird ? "UFO" : pl->m_isBall ? "Ball" :
                               pl->m_isDart ? "Wave" : pl->m_isRobot ? "Robot" : pl->m_isSpider ? "Spider" :
                               pl->m_isSwing ? "Swing" : "Cube";
            s += fmt::format("Mode: {}\n", mode);
        }
        if (on("lbl_clock")) {
            auto t = std::time(nullptr); char b[16];
            std::strftime(b, sizeof b, "%H:%M:%S", std::localtime(&t));
            s += fmt::format("Clock: {}\n", b);
        }
        bool right = on("lbl_right");
        auto ws = CCDirector::get()->getWinSize();
        f->label->setString(s.c_str());
        f->label->setScale(val("lbl_scale"));
        f->label->setOpacity(static_cast<GLubyte>(val("lbl_opacity") * 255.f));
        f->label->setAnchorPoint({right ? 1.f : 0.f, 1.f});
        f->label->setAlignment(right ? kCCTextAlignmentRight : kCCTextAlignmentLeft);
        f->label->setPosition(right ? ws.width - 6.f : 6.f, ws.height - 6.f);
    }

    void destroyPlayer(PlayerObject* p, GameObject* obj) {
        auto f = m_fields.self();
        if (obj != m_anticheatSpike) {
            // Destroy Hazards: break up to N hazards per attempt
            if (on("destroy_on") && obj && obj->m_objectType == GameObjectType::Hazard &&
                f->broken.size() < static_cast<size_t>(val("destroy_count"))) {
                f->broken.push_back({obj, obj->m_objectType});
                obj->m_objectType = GameObjectType::Decoration;
                obj->setVisible(false);
                return;
            }
            // Noclip
            bool noclipApplies = (p == m_player2) ? on("noclip_p2") : on("noclip_p1");
            bool solidKills = obj && obj->m_objectType == GameObjectType::Solid && on("noclip_solid_dies");
            if (on("noclip") && noclipApplies && !solidKills) {
                if (!f->hit) f->deaths++;
                f->hit = true;
                float minAcc = val("noclip_min_acc");
                float acc = 100.f * (1.f - static_cast<float>(f->badFrames + 1) / std::max(1, f->frames));
                if (!(minAcc > 0.f && f->frames > 60 && acc < minAcc)) return;
            }
        }
        g_sessionDeaths++;
        PlayLayer::destroyPlayer(p, obj);
    }

    void levelComplete() {
        // Auto Safe Mode: run is treated as a test run, nothing is saved
        if (on("safe_mode") && cheating()) {
            m_isTestMode = true;
            if (on("safe_notify"))
                Notification::create("Safe Mode: this run will not be saved", NotificationIcon::Info)->show();
        }
        PlayLayer::levelComplete();
    }
};
