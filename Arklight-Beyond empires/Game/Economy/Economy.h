/* OGame Beyond - Economy System
 * Developer: Stephen
 * Empire-wide economic management, trade routes, and market.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include "Resources.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {

struct TradeRoute {
    int id = -1;
    int fromPlanetID = -1;
    int toPlanetID = -1;
    int ownerEmpireID = -1;
    std::string resourceType;
    float volume = 100;
    float price = 5.0f;
    float tariff = 0.1f;
    bool isActive = true;
};

struct MarketOrder {
    int id = -1;
    std::string resourceType;
    int ownerEmpireID = -1;
    float price = 0;
    int quantity = 0;
    bool isBuyOrder = true;
    bool isFilled = false;
};

struct EconomyStats {
    float totalIncome = 0;
    float totalExpenses = 0;
    float balance = 0;
    float tradeVolume = 0;
    float taxRevenue = 0;
    float maintenanceCost = 0;
    float constructionCost = 0;
    float researchCost = 0;
    float fleetUpkeep = 0;
};

class EconomySystem {
public:
    std::vector<TradeRoute> tradeRoutes;
    std::vector<MarketOrder> marketOrders;
    std::unordered_map<int, EconomyStats> empireEconomics; // empireID -> stats

    void Init() {}

    void Update(float dt) {
        for (auto& [empireID, stats] : empireEconomics) {
            stats.balance = stats.totalIncome - stats.totalExpenses;
        }
    }

    void AddIncome(int empireID, float amount, const std::string& source = "") {
        empireEconomics[empireID].totalIncome += amount;
    }

    void AddExpense(int empireID, float amount, const std::string& source = "") {
        empireEconomics[empireID].totalExpenses += amount;
    }

    float GetBalance(int empireID) const {
        auto it = empireEconomics.find(empireID);
        return it != empireEconomics.end() ? it->second.balance : 0;
    }

    void CreateTradeRoute(int fromPlanet, int toPlanet, int empireID, const std::string& resource, float volume) {
        TradeRoute route;
        route.id = static_cast<int>(tradeRoutes.size());
        route.fromPlanetID = fromPlanet;
        route.toPlanetID = toPlanet;
        route.ownerEmpireID = empireID;
        route.resourceType = resource;
        route.volume = volume;
        tradeRoutes.push_back(route);
    }

    void ProcessTradeRoutes(float dt) {
        for (auto& route : tradeRoutes) {
            if (route.isActive) {
                AddIncome(route.ownerEmpireID, route.volume * route.price * dt * 0.01f, "trade");
            }
        }
    }
};

} // namespace ogb
