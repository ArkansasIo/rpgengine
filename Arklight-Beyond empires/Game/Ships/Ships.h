/* ArkLight Beyond - Ships
 * Developer: Stephen
 * Ship classes, components, and damage model.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>

namespace ogb {
enum class ShipClass {
    Civilian, Military, Support, Transport, Capital, Titan, Carrier,
    Science, Mining, Construction, Fighter, Bomber, Corvette, Frigate,
    Cruiser, Battleship, Dreadnought, Colony
};

struct ShipDef {
    std::string id;
    std::string name;
    ShipClass shipClass = ShipClass::Military;
    float hull = 100;
    float armor = 0;
    float shield = 0;
    float shieldRegen = 0;
    float speed = 10;
    float turnRate = 90;
    float cargoCapacity = 0;
    float fuelCapacity = 100;
    float crewCapacity = 10;
    float powerOutput = 100;
    float buildCostMetal = 100;
    float buildCostCrystal = 50;
    float buildTime = 10;
    int requiredDockLevel = 1;
    std::string icon;
    std::vector<std::string> weaponSlots;
    std::vector<std::string> moduleSlots;
};

struct ShipInstance {
    int id = -1;
    std::string defID;
    std::string name;
    int ownerEmpireID = -1;
    int fleetID = -1;
    float hull = 100;
    float maxHull = 100;
    float armor = 0;
    float shield = 0;
    float maxShield = 100;
    float shieldRegen = 5;
    float speed = 10;
    float currentSpeed = 0;
    float fuel = 100;
    float maxFuel = 100;
    int experience = 0;
    int level = 1;
    bool isAlive = true;
    bool isSelected = false;
    Vec3 position;
    Vec3 velocity;
    float heading = 0;
    std::vector<std::string> equippedWeapons;
    std::vector<std::string> equippedModules;

    void UpdateShield(float dt) {
        if (shield < maxShield) {
            shield = std::min(maxShield, shield + shieldRegen * dt);
        }
    }

    float TakeDamage(float damage, float armorPen = 0) {
        float armorReduction = armor * (1.0f - armorPen);
        float shieldAbsorb = std::min(shield, damage);
        shield -= shieldAbsorb;
        float hullDamage = std::max(0.0f, damage - shieldAbsorb - armorReduction);
        hull -= hullDamage;
        if (hull <= 0) { hull = 0; isAlive = false; }
        return hullDamage;
    }
};

class ShipRegistry {
public:
    void Init() {
        Add({"scout", "Scout", ShipClass::Civilian, 30, 0, 0, 0, 20, 120, 20, 5, 10, 50, 50, 1, "🔍", {}, {}});
        Add({"fighter", "Fighter", ShipClass::Fighter, 50, 5, 20, 2, 25, 90, 10, 30, 120, 80, 2, "✈", {"laser"}, {}});
        Add({"corvette", "Corvette", ShipClass::Corvette, 100, 15, 50, 5, 15, 60, 20, 80, 200, 120, 3, "🔲", {"laser","missile"}, {"shield_gen"}});
        Add({"frigate", "Frigate", ShipClass::Frigate, 200, 30, 100, 10, 12, 45, 40, 150, 350, 200, 4, "🛡", {"laser","missile","torpedo"}, {"shield_gen","engine_boost"}});
        Add({"cruiser", "Cruiser", ShipClass::Cruiser, 400, 50, 200, 15, 10, 30, 80, 300, 600, 350, 5, "⚔", {"laser","missile","torpedo","flak"}, {"shield_gen","engine_boost","repair_bot"}});
        Add({"battleship", "Battleship", ShipClass::Battleship, 800, 100, 400, 20, 8, 20, 150, 500, 1000, 500, 6, "🏰", {"laser","missile","torpedo","flak","railgun"}, {"shield_gen","engine_boost","repair_bot","damage_boost"}});
        Add({"carrier", "Carrier", ShipClass::Carrier, 600, 80, 300, 15, 6, 15, 200, 400, 800, 400, 6, "🛩", {"laser","flak"}, {"fighter_bay","shield_gen","repair_bot"}});
        Add({"colony_ship", "Colony Ship", ShipClass::Colony, 150, 10, 0, 0, 8, 30, 500, 200, 250, 150, 4, "🏠", {}, {"colony_module"}});
        Add({"mining_ship", "Mining Ship", ShipClass::Mining, 80, 5, 0, 0, 12, 40, 100, 60, 150, 80, 2, "⛏", {}, {"mining_laser"}});
        Add({"titan", "Titan", ShipClass::Titan, 2000, 200, 800, 30, 5, 10, 300, 800, 2000, 800, 8, "👑", {"laser","missile","torpedo","flak","railgun","nova_cannon"}, {"shield_gen","engine_boost","repair_bot","damage_boost","fighter_bay"}});
    }

    const ShipDef* Get(const std::string& id) const {
        auto it = defs.find(id); return it != defs.end() ? &it->second : nullptr;
    }

private:
    std::unordered_map<std::string, ShipDef> defs;
    void Add(const ShipDef& def) { defs[def.id] = def; }
};
} // namespace ogb
