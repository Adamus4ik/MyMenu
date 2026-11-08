#include "cheats.hpp"
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

class CheatMenu : public Popup<> {
    std::vector<CCLabelBMFont*> m_vals;
    static inline CheatMenu* s_open = nullptr;

protected:
    bool setup() override {
        s_open = this;
        this->setTitle("My Menu  (TAB = close)");
        auto& reg = registry();
        float W = m_size.width - 30.f, H = m_size.height - 70.f;
        const float ROW = 28.f;

        size_t headers = 0; std::string last;
        for (auto& c : reg) if (c.cat != last) { last = c.cat; headers++; }
        float contentH = std::max(H, (reg.size() + headers + 2) * ROW + 10.f);

        auto scroll = ScrollLayer::create({W, H});
        scroll->setPosition({15.f, 12.f});
        m_mainLayer->addChild(scroll);
        auto content = scroll->m_contentLayer;
        content->setContentSize({W, contentH});

        auto menu = CCMenu::create();
        menu->setPosition(0.f, 0.f);
        content->addChild(menu);

        float y = contentH - 20.f;
        auto rspr = ButtonSprite::create("Reset all");
        rspr->setScale(0.6f);
        auto rbtn = CCMenuItemSpriteExtra::create(rspr, this, menu_selector(CheatMenu::onReset));
        rbtn->setPosition(W / 2.f, y);
        menu->addChild(rbtn);
        y -= ROW;

        m_vals.assign(reg.size(), nullptr);
        std::string cat;
        for (size_t i = 0; i < reg.size(); i++) {
            auto& c = reg[i];
            if (c.cat != cat) {
                cat = c.cat;
                auto h = CCLabelBMFont::create(cat.c_str(), "goldFont.fnt");
                h->setScale(0.55f); h->setAnchorPoint({0.f, 0.5f}); h->setPosition(8.f, y);
                content->addChild(h);
                y -= ROW;
            }
            auto l = CCLabelBMFont::create(c.name.c_str(), "bigFont.fnt");
            l->setAnchorPoint({0.f, 0.5f});
            l->limitLabelWidth(W * 0.55f, 0.42f, 0.2f);
            l->setPosition(14.f, y);
            content->addChild(l);

            if (c.toggle) {
                auto t = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(CheatMenu::onToggle), 0.55f);
                t->toggle(on(c.id));
                t->setTag(static_cast<int>(i));
                t->setPosition(W - 30.f, y);
                menu->addChild(t);
            } else {
                auto mk = [&](char const* txt, int tag, float x) {
                    auto s = ButtonSprite::create(txt);
                    s->setScale(0.5f);
                    auto b = CCMenuItemSpriteExtra::create(s, this, menu_selector(CheatMenu::onStep));
                    b->setTag(tag); b->setPosition(x, y); menu->addChild(b);
                };
                mk("-", static_cast<int>(i) * 2, W - 125.f);
                mk("+", static_cast<int>(i) * 2 + 1, W - 35.f);
                auto v = CCLabelBMFont::create(fmt::format("{:.2f}", val(c.id)).c_str(), "bigFont.fnt");
                v->setScale(0.4f); v->setPosition(W - 80.f, y);
                content->addChild(v);
                m_vals[i] = v;
            }
            y -= ROW;
        }
        content->setPositionY(H - contentH);
        return true;
    }

    void onToggle(CCObject* s) {
        auto t = static_cast<CCMenuItemToggler*>(s);
        auto& c = registry()[t->getTag()];
        setVal(c.id, t->isToggled() ? 0.f : 1.f); // callback runs before the state flips
        onCheatChanged(c.id);
    }
    void onStep(CCObject* s) {
        int tag = static_cast<CCNode*>(s)->getTag();
        auto& c = registry()[tag / 2];
        float step = std::max(0.01f, (c.max - c.min) / 20.f);
        float v = std::clamp(val(c.id) + ((tag % 2) ? step : -step), c.min, c.max);
        setVal(c.id, v);
        m_vals[tag / 2]->setString(fmt::format("{:.2f}", v).c_str());
        onCheatChanged(c.id);
    }
    void onReset(CCObject*) {
        for (auto& c : registry()) { setVal(c.id, c.def); onCheatChanged(c.id); }
        CheatMenu::toggle(); // close
        CheatMenu::toggle(); // reopen with fresh values
    }
    void onClose(CCObject* o) override {
        s_open = nullptr;
        Popup<>::onClose(o);
    }

public:
    static CheatMenu* create() {
        auto r = new CheatMenu();
        if (r && r->initAnchored(400.f, 280.f)) { r->autorelease(); return r; }
        CC_SAFE_DELETE(r);
        return nullptr;
    }
    static void toggle() {
        if (s_open) s_open->onClose(nullptr);
        else if (auto m = create()) m->show();
    }
};

// TAB opens / closes the menu. If the build complains about the signature, remove the
// last "double" parameter (older GD versions do not have it).
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat, double ts) {
        if (down && !repeat && key == KEY_Tab) { CheatMenu::toggle(); return true; }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, ts);
    }
};
