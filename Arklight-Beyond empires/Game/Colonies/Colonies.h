/* ArkLight Beyond - Colony System
 * Developer: Stephen
 * Planet colonization, management, and development.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {
struct ColonyDef {
    std::string id;
    std::string planetType;
    float habitability = 1.0f;
    int maxPopulation = 10000;
    float metalOutput = 10;
    float crystalOutput = 5;
    float foodOutput = 8;
    float energyOutput = 20;
};

struct ColonyInstance {
    int id = -1;
    std::string name;
    int planetID = -1;
    int ownerEmpireID = -1;
    int population = 1000;
    int maxPopulation = 10000;
    float morale = 75.0f;
    float happiness = 80.0f;
    float crime = 5.0f;
    float pollution = 0.0f;
    float unrest = 0.0f;
    int buildingSlots = 10;
    int usedBuildingSlots = 0;
    std::vector<int> buildingIDs;
    bool isCapital = false;

    float GetGrowthRate() const {
        float growth = 1.0f;
        if (morale > 80) growth *= 1.2f;
        else if (morale < 30) growth *= 0.5f;
        if (population >= maxPopulation * 0.9f) growth *= 0.1f;
        return growth;
    }

    float GetTaxRevenue() const { return population * 0.1f; }
    float GetHappinessFactor() const { return happiness / 100.0f; }
};

class ColonyManager {
public:
    std::vector<ColonyInstance> colonies;
    int nextColonyID = 1;

    int CreateColony(const std::string& name, int planetID, int empireID) {
        ColonyInstance c;
        c.id = nextColonyID++;
        c.name = name;
        c.planetID = planetID;
        c.ownerEmpireID = empireID;
        colonies.push_back(c);
        return c.id;
    }

    ColonyInstance* GetColony(int id) {
        for (auto& c : colonies) if (c.id == id) return &c;
        return nullptr;
    }

    std::vector<ColonyInstance*> GetColoniesForEmpire(int empireID) {
        std::vector<ColonyInstance*> result;
        for (auto& c : colonies) if (c.ownerEmpireID == empireID) result.push_back(&c);
        return result;
    }

    void Update(float dt) {
        for (auto& c : colonies) {
            // Population growth
            float growthRate = c.GetGrowthRate();
            c.population = std::min(c.maxPopulation, c.population + static_cast<int>(growthRate * dt));

            // Morale decay
            if (c.crime > 20) c.morale -= 0.5f * dt;
            if (c.pollution > 30) c.morale -= 0.3f * dt;
            c.morale = std::max(0.0f, std::min(100.0f, c.morale));

            // Unrest
            if (c.morale < 20) c.unrest += 2.0f * dt;
            else c.unrest -= 1.0f * dt;
            c.unrest = std::max(0.0f, std::min(100.0f, c.unrest));
        }
    }

    float GetTotalPopulation(int empireID) const {
        float total = 0;
        for (auto& c : colonies) if (c.ownerEmpireID == empireID) total += c.population;
        return total;
    }
};

} // namespace ogb
