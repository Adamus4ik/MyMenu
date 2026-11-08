#include "cheats.hpp"

static void T(char const* id, char const* name, char const* cat, bool def = false) {
    registry().push_back({id, name, cat, true, def ? 1.f : 0.f, 0.f, 1.f});
}
static void S(char const* id, char const* name, char const* cat, float def, float mn, float mx) {
    registry().push_back({id, name, cat, false, def, mn, mx});
}

$execute {
    // Player (15)
    T("speed_on", "Speedhack", "Player");
    S("speed_val", "Speed multiplier", "Player", 1.f, 0.05f, 5.f);
    T("fps_on", "FPS bypass", "Player");
    S("fps_val", "FPS value", "Player", 60.f, 30.f, 360.f);
    T("noclip", "Noclip", "Player");
    T("noclip_label", "Noclip Accuracy / Deaths", "Player", true);
    S("noclip_min_acc", "Die below accuracy % (0=off)", "Player", 0.f, 0.f, 100.f);
    T("safe_mode", "Auto Safe Mode", "Player", true);
    T("destroy_on", "Destroy Hazards", "Player");
    S("destroy_count", "Hazards to destroy", "Player", 3.f, 1.f, 50.f);
    T("input_buffer", "Inputs buffer", "Player");
    S("input_buffer_frames", "Buffer window (frames)", "Player", 6.f, 1.f, 30.f);
    T("hide_p1", "Hide Player 1", "Player");
    T("hide_p2", "Hide Player 2", "Player");
    T("no_death_effect", "No death effect", "Player");
    // Practice (3)
    T("force_practice", "Force practice mode", "Practice");
    T("auto_checkpoint", "Auto checkpoints", "Practice");
    S("checkpoint_secs", "Checkpoint every (sec)", "Practice", 3.f, 1.f, 20.f);
    // Bots (6)
    T("autopilot", "Autopilot (simple cube levels)", "Bots");
    S("autopilot_look", "Autopilot jump distance", "Bots", 45.f, 20.f, 120.f);
    S("autopilot_hold", "Autopilot hold (frames)", "Bots", 3.f, 1.f, 10.f);
    T("autoclick", "Autoclicker", "Bots");
    S("autoclick_cps", "Autoclicker CPS", "Bots", 10.f, 1.f, 30.f);
    T("autohold", "Auto hold button", "Bots");
    // Visual (2)
    T("show_layout", "Show Layout", "Visual");
    T("show_hitboxes", "Show Hitboxes", "Visual");
    // Labels (16)
    T("lbl_cheat", "CHEATS ON indicator", "Labels", true);
    T("lbl_fps", "FPS", "Labels");
    T("lbl_cps", "CPS", "Labels");
    T("lbl_frames", "Frame counter", "Labels");
    T("lbl_time", "Attempt time", "Labels");
    T("lbl_percent", "Percent", "Labels");
    T("lbl_attempt", "Attempt number", "Labels");
    T("lbl_best", "Best percent", "Labels");
    T("lbl_level", "Level name", "Labels");
    T("lbl_pos", "Player position", "Labels");
    T("lbl_speed", "Speed multiplier", "Labels");
    T("lbl_session", "Session deaths", "Labels");
    T("lbl_clock", "Real clock", "Labels");
    T("lbl_right", "Labels on right side", "Labels");
    S("lbl_scale", "Label size", "Labels", 0.4f, 0.2f, 1.f);
    S("lbl_opacity", "Label opacity", "Labels", 0.6f, 0.1f, 1.f);
    // Unlock (2)
    T("unlock_icons", "Unlock all icons", "Unlock");
    T("unlock_colors", "Unlock all colors", "Unlock");
    // Generator (6)
    S("gen_len", "Generator: segments", "Generator", 40.f, 10.f, 200.f);
    T("gen_decor", "Generator: decorations", "Generator", true);
    T("gen_double", "Generator: double spikes", "Generator", true);
    T("gen_steps", "Generator: step blocks", "Generator", true);
    S("gen_gap", "Generator: gap (blocks)", "Generator", 8.f, 6.f, 14.f);
    S("gen_density", "Generator: decor density %", "Generator", 40.f, 0.f, 100.f);

    for (auto& c : registry())
        cache()[c.id] = Mod::get()->getSavedValue<float>("c_" + c.id, c.def);
}
