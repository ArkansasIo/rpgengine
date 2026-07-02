/* OGame Beyond - Buildings
 * Developer: Stephen
 * Building definitions, construction, and production.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {
struct BuildingDef {
    std::string id;
    std::string name;
    std::string description;
    std::string category; // "Resource", "Military", "Economy", "Research", "Defense"
    std::string icon;
    float health = 100;
    float armor = 0;
    float energyUsage = 0;
    float energyProduction = 0;
    int workersRequired = 0;
    float constructionTime = 10;
    float upgradeCostMetal = 0;
    float upgradeCostCrystal = 0;
    float maintenanceCost = 0;
    float storageCapacity = 0;
    int maxLevel = 10;
    std::vector<std::string> requiredTech;
    std::vector<std::string> prerequisites; // building IDs needed first
};

struct BuildingInstance {
    int id = -1;
    std::string defID;
    int level = 1;
    float health = 100;
    float constructionProgress = 0;
    bool isConstructing = false;
    bool isProducing = false;
    int ownerEmpireID = -1;
    int planetID = -1;
    std::vector<std::string> productionQueue;
};

class BuildingRegistry {
public:
    void Init() {
        Register({"metal_mine", "Metal Mine", "Extracts metal ore", "Resource", "⛏", 100, 0, 10, 0, 2, 8, 50, 25, 5, 1000, {}, {}});
        Register({"crystal_mine", "Crystal Mine", "Extracts crystals", "Resource", "💎", 80, 0, 15, 0, 3, 12, 75, 35, 10, 800, {}, {"metal_mine"}});
        Register({"deuterium_synth", "Deuterium Synthesizer", "Produces deuterium fuel", "Resource", "🧪", 120, 0, 25, 0, 4, 15, 100, 50, 15, 500, {"fusion_tech"}, {"metal_mine"}});
        Register({"solar_plant", "Solar Plant", "Generates energy", "Resource", "☀", 60, 0, 0, 50, 1, 5, 30, 15, 0, 0, {}, {}});
        Register({"fusion_reactor", "Fusion Reactor", "High-output energy", "Resource", "🔬", 200, 10, 0, 200, 5, 20, 200, 100, 0, 0, {"fusion_tech"}, {"solar_plant"}});
        Register({"shipyard", "Shipyard", "Constructs ships", "Military", "🏗", 300, 20, 30, 0, 8, 25, 150, 75, 0, 0, {"ship_construction"}, {"metal_mine"}});
        Register({"research_lab", "Research Lab", "Conducts research", "Research", "🔬", 150, 0, 20, 0, 5, 10, 80, 40, 0, 0, {}, {"metal_mine"}});
        Register({"missile_silo", "Missile Silo", "Launches orbital strikes", "Military", "🚀", 400, 50, 40, 0, 6, 30, 200, 100, 0, 0, {"missile_tech"}, {"research_lab"}});
        Register({"orbital_cannon", "Orbital Cannon", "Planetary defense", "Defense", "🛡", 500, 100, 50, 0, 10, 40, 300, 150, 0, 0, {"orbital_tech"}, {"research_lab"}});
        Register({"trade_hub", "Trade Hub", "Increases trade income", "Economy", "📊", 100, 0, 15, 30, 3, 8, 60, 30, 20, 0, {"trade_tech"}, {}});
        Register({"embassy", "Embassy", "Enables diplomacy", "Diplomacy", "🏛", 80, 0, 10, 0, 2, 6, 40, 20, 0, 0, {}, {}});
        Register({"nanite_factory", "Nanite Factory", "Auto-repairs structures", "Support", "🔧", 250, 0, 35, 0, 8, 30, 180, 90, 0, 0, {"nanite_tech"}, {"research_lab"}});
        Register({"command_center", "Command Center", "Main empire building", "Command", "🏰", 1000, 50, 0, 0, 10, 0, 500, 250, 0, 0, {}, {}});
    }

    const BuildingDef* Get(const std::string& id) const {
        auto it = defs.find(id); return it != defs.end() ? &it->second : nullptr;
    }

    std::vector<BuildingDef*> GetByCategory(const std::string& cat) {
        std::vector<BuildingDef*> result;
        for (auto& [id, def] : defs) if (def.category == cat) result.push_back(&def);
        return result;
    }

private:
    std::unordered_map<std::string, BuildingDef> defs;
    void Register(const BuildingDef& def) { defs[def.id] = def; }
};
} // namespace ogb
