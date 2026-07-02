/* ArcLight Engine - Trade System
 * Developer: Stephen
 * Handles economy, resource trading, market prices, and supply/demand.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>

namespace arclight {

struct ResourceType {
	std::string id;
	std::string name;
	float basePrice = 1.0f;
	float weight = 1.0f;
	bool isTradable = true;
	bool isRefinable = false;
	std::string refinedTypeID;
	float refinedMultiplier = 2.0f;
};

struct MarketOffer {
	std::string resourceID;
	std::string sellerID;
	float price;
	int quantity;
	bool isBuyOrder; // true = buying, false = selling
};

struct TradeRoute {
	std::string fromFaction;
	std::string toFaction;
	std::string resourceID;
	float tariffRate = 0.1f; // 10% tax
	bool isActive = true;
};

class TradeSystem {
public:
	void RegisterResource(const ResourceType& res) {
		resources[res.id] = res;
		marketPrices[res.id] = res.basePrice;
		marketSupply[res.id] = 1000;
		marketDemand[res.id] = 1000;
	}

	void SetMarketPrice(const std::string& resourceID, float price) {
		marketPrices[resourceID] = price;
	}

	float GetMarketPrice(const std::string& resourceID) const {
		auto it = marketPrices.find(resourceID);
		return (it != marketPrices.end()) ? it->second : 0.0f;
	}

	void UpdateSupplyDemand(const std::string& resourceID, float supplyDelta, float demandDelta) {
		marketSupply[resourceID] = std::max(0.0f, marketSupply[resourceID] + supplyDelta);
		marketDemand[resourceID] = std::max(0.0f, marketDemand[resourceID] + demandDelta);

		// Price adjusts based on supply/demand ratio
		float supply = marketSupply[resourceID];
		float demand = marketDemand[resourceID];
		float ratio = (supply > 0) ? demand / supply : 10.0f;
		float basePrice = resources[resourceID].basePrice;
		marketPrices[resourceID] = basePrice * std::max(0.1f, std::min(10.0f, ratio));
	}

	float ExecuteTrade(const std::string& resourceID, int quantity, bool isBuying, const std::string& factionID) {
		if (quantity <= 0) return 0.0f;

		float price = GetMarketPrice(resourceID);
		float totalCost = price * quantity;

		// Apply tariffs for cross-faction trade
		auto it = tradeRoutes.find({factionID, resourceID});
		if (it != tradeRoutes.end() && it->second.isActive) {
			totalCost *= (1.0f + it->second.tariffRate);
		}

		if (isBuying) {
			UpdateSupplyDemand(resourceID, -quantity, quantity);
		} else {
			UpdateSupplyDemand(resourceID, quantity, -quantity);
		}

		if (onTradeExecuted) onTradeExecuted(resourceID, quantity, isBuying, totalCost, factionID);
		return totalCost;
	}

	void CreateTradeRoute(const TradeRoute& route) {
		tradeRoutes[{route.fromFaction, route.resourceID}] = route;
	}

	void Update(float dt) {
		// Market fluctuation
		for (auto& [id, price] : marketPrices) {
			float fluctuation = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.02f;
			price *= (1.0f + fluctuation);
			price = std::max(0.01f, price);
		}
	}

	std::function<void(const std::string&, int, bool, float, const std::string&)> onTradeExecuted;

private:
	std::unordered_map<std::string, ResourceType> resources;
	std::unordered_map<std::string, float> marketPrices;
	std::unordered_map<std::string, float> marketSupply;
	std::unordered_map<std::string, float> marketDemand;
	std::unordered_map<std::pair<std::string, std::string>, TradeRoute> tradeRoutes;
};

} // namespace arclight
