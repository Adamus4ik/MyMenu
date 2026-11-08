#include "cheats.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <random>

// Rule-based procedural level generator (not an LLM). Keeps gaps wide so it stays beatable.
class $modify(GenEditorUI, EditorUI) {
    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;
        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Gen"), this, menu_selector(GenEditorUI::onGenerate));
        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition(50.f, CCDirector::get()->getWinSize().height - 40.f);
        this->addChild(menu, 100);
        return true;
    }

    void onGenerate(CCObject*) {
        auto lel = m_editorLayer;
        int segments = static_cast<int>(val("gen_len"));
        float gap = val("gen_gap") * 30.f;
        int density = static_cast<int>(val("gen_density"));
        std::mt19937 rng(std::random_device{}());
        auto roll = [&](int a, int b) { return std::uniform_int_distribution<int>(a, b)(rng); };

        std::vector<int> kinds{0, 3};
        if (on("gen_double")) kinds.push_back(1);
        if (on("gen_steps")) kinds.push_back(2);

        constexpr float G = 15.f;
        constexpr int BLOCK = 1, SPIKE = 8;
        float x = 400.f;
        for (int i = 0; i < segments; i++) {
            switch (kinds[roll(0, static_cast<int>(kinds.size()) - 1)]) {
                case 0: lel->createObject(SPIKE, {x, G}, false); break;
                case 1:
                    lel->createObject(SPIKE, {x, G}, false);
                    lel->createObject(SPIKE, {x + 30.f, G}, false);
                    break;
                case 2:
                    lel->createObject(BLOCK, {x, G}, false);
                    lel->createObject(SPIKE, {x + 60.f, G}, false);
                    break;
                default: break;
            }
            if (on("gen_decor") && roll(0, 99) < density)
                lel->createObject(BLOCK, {x + roll(0, 60), 210.f + roll(0, 4) * 30.f}, false);
            x += gap + roll(0, 3) * 30.f;
        }
        FLAlertLayer::create("Gen", fmt::format("Created {} segments", segments), "OK")->show();
    }
};
