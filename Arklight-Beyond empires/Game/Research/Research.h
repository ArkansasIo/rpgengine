/* ArkLight Beyond - Research Tree
 * Developer: Stephen
 * Technology tree, research projects, and prerequisites.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {
struct TechDef {
    std::string id;
    std::string name;
    std::string description;
    std::string category; // Physics, Engineering, Biology, Computing, Weapons, etc.
    int tier = 1;
    float researchCost = 100;
    float researchTime = 10;
    std::vector<std::string> prerequisites;
    std::vector<std::string> unlocks; // building/ship IDs
    std::string icon;
    bool isRepeatable = false;
};

struct ResearchProject {
    int id = -1;
    std::string techID;
    int ownerEmpireID = -1;
    float progress = 0;
    float totalCost = 0;
    bool isActive = false;
    int assignedScientists = 0;
};

class ResearchRegistry {
public:
    void Init() {
        // Physics
        Add({"basic_physics", "Basic Physics", "Fundamental physics", "Physics", 1, 50, 5, {}, {"laser_basic","shield_basic"}, "⚛"});
        Add({"laser_basic", "Basic Lasers", "Simple laser weapons", "Weapons", 1, 80, 8, {"basic_physics"}, {"laser_turret"}, "🔫"});
        Add({"shield_basic", "Basic Shields", "Energy shields", "Defense", 1, 100, 10, {"basic_physics"}, {"shield_generator"}, "🛡"});
        Add({"fusion_tech", "Fusion Technology", "Nuclear fusion power", "Engineering", 2, 200, 20, {"basic_physics"}, {"fusion_reactor"}, "🔬"});
        Add({"missile_tech", "Missile Technology", "Guided missiles", "Weapons", 2, 150, 15, {"laser_basic"}, {"missile_launcher"}, "🚀"});
        Add({"orbital_tech", "Orbital Technology", "Space-based structures", "Engineering", 3, 300, 30, {"fusion_tech"}, {"orbital_cannon","space_dock"}, "🛸"});
        Add({"nanite_tech", "Nanite Technology", "Self-replicating nanobots", "Biology", 4, 500, 40, {"fusion_tech"}, {"nanite_factory"}, "🔬"});
        Add({"trade_tech", "Advanced Trade", "Interstellar commerce", "Economy", 2, 120, 12, {}, {"trade_hub"}, "📊"});
        Add({"ship_construction", "Ship Construction", "Build military vessels", "Engineering", 1, 100, 10, {}, {"shipyard"}, "🏗"});
    }

    const TechDef* Get(const std::string& id) const {
        auto it = defs.find(id); return it != defs.end() ? &it->second : nullptr;
    }

    std::vector<TechDef*> GetByCategory(const std::string& cat) {
        std::vector<TechDef*> result;
        for (auto& [id, def] : defs) if (def.category == cat) result.push_back(&def);
        return result;
    }

private:
    std::unordered_map<std::string, TechDef> defs;
    void Add(const TechDef& def) { defs[def.id] = def; }
};
} // namespace ogb
