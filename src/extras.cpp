// Riskier hooks live here. If the build fails with errors pointing to this file,
// delete it: the rest of the mod still builds (those menu items just do nothing).
#include "cheats.hpp"
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/PlayerObject.hpp>

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
