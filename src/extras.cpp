// Riskier hooks live here. If the build fails with errors pointing to this file,
// delete it: the rest of the mod still builds (those menu items just do nothing).
#include "cheats.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/PlayerObject.hpp>

class $modify(ExtraGL, GJBaseGameLayer) {
    void updateVisibility(float dt) {
        GJBaseGameLayer::updateVisibility(dt);
        if (!PlayLayer::get()) return;
        m_isDebugDrawEnabled = on("show_hitboxes");
        if (m_isDebugDrawEnabled) this->updateDebugDraw();
        if (on("show_layout")) {
            for (auto o : CCArrayExt<GameObject*>(m_objects))
                if (o->m_objectType == GameObjectType::Decoration) o->setVisible(false);
        }
    }
};

class $modify(ExtraGM, GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        return on("unlock_icons") ? true : GameManager::isIconUnlocked(id, type);
    }
    bool isColorUnlocked(int id, UnlockType type) {
        return on("unlock_colors") ? true : GameManager::isColorUnlocked(id, type);
    }
};

class $modify(ExtraPO, PlayerObject) {
    void playDeathEffect() {
        if (on("no_death_effect")) return;
        PlayerObject::playDeathEffect();
    }
};
