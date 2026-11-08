#pragma once
#include <Geode/Geode.hpp>
#include <unordered_map>
using namespace geode::prelude;

struct Cheat { std::string id, name, cat; bool toggle; float def, min, max; };

inline std::vector<Cheat>& registry() { static std::vector<Cheat> r; return r; }
inline std::unordered_map<std::string, float>& cache() { static std::unordered_map<std::string, float> c; return c; }

inline float val(std::string const& id) {
    auto& c = cache(); auto it = c.find(id);
    return it == c.end() ? 0.f : it->second;
}
inline bool on(std::string const& id) { return val(id) != 0.f; }
inline void setVal(std::string const& id, float v) {
    cache()[id] = v;
    Mod::get()->setSavedValue<float>("c_" + id, v);
}
void onCheatChanged(std::string const& id); // core.cpp
bool cheating();                            // core.cpp
