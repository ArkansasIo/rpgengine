/* ArkLight Beyond - Resources
 * Developer: Stephen
 * All resource types and economic management.
 */
#pragma once
#include <string>
#include <unordered_map>

namespace ogb {
enum class ResourceType {
    Metal, Crystal, Deuterium, Energy, DarkMatter,
    Influence, Credits, Population, Food, Morale,
    ResearchPoints, ProductionPoints
};

struct ResourceDef {
    ResourceType type;
    std::string name;
    std::string icon;
    float basePrice = 1.0f;
    float maxStorage = 10000.0f;
    float currentAmount = 0;
    float perTick = 0;
    float capacity = 10000.0f;
};

class ResourceManager {
public:
    void Init() {
        Add(ResourceType::Metal, "Metal", "⛏", 5.0f, 50000);
        Add(ResourceType::Crystal, "Crystal", "💎", 8.0f, 30000);
        Add(ResourceType::Deuterium, "Deuterium", "🧪", 12.0f, 20000);
        Add(ResourceType::Energy, "Energy", "⚡", 1.0f, 100000);
        Add(ResourceType::DarkMatter, "Dark Matter", "🌑", 25.0f, 5000);
        Add(ResourceType::Influence, "Influence", "👑", 1.0f, 1000);
        Add(ResourceType::Credits, "Credits", "💰", 0.1f, 1000000);
        Add(ResourceType::Population, "Population", "👥", 0, 10000);
        Add(ResourceType::Food, "Food", "🌾", 2.0f, 50000);
        Add(ResourceType::Morale, "Morale", "😊", 0, 100);
        Add(ResourceType::ResearchPoints, "Research", "🔬", 0, 10000);
        Add(ResourceType::ProductionPoints, "Production", "🏭", 0, 10000);
    }

    void Add(ResourceType type, const std::string& name, const std::string& icon, float price, float cap) {
        ResourceDef def; def.type = type; def.name = name; def.icon = icon;
        def.basePrice = price; def.maxStorage = cap; def.capacity = cap;
        resources[type] = def;
    }

    float Get(ResourceType type) const {
        auto it = resources.find(type); return it != resources.end() ? it->second.currentAmount : 0;
    }

    void AddAmount(ResourceType type, float amount) {
        auto it = resources.find(type);
        if (it != resources.end()) {
            it->second.currentAmount = std::min(it->second.capacity, it->second.currentAmount + amount);
        }
    }

    bool Spend(ResourceType type, float amount) {
        auto it = resources.find(type);
        if (it != resources.end() && it->second.currentAmount >= amount) {
            it->second.currentAmount -= amount;
            return true;
        }
        return false;
    }

    void Update(float dt) {
        for (auto& [type, res] : resources) {
            res.currentAmount = std::max(0.0f, std::min(res.capacity, res.currentAmount + res.perTick * dt));
        }
    }

    ResourceDef* GetDef(ResourceType type) {
        auto it = resources.find(type); return it != resources.end() ? &it->second : nullptr;
    }

    float GetMarketPrice(ResourceType type) const {
        auto it = resources.find(type);
        if (it == resources.end()) return 0;
        float supply = it->second.currentAmount;
        float demand = it->second.perTick;
        float ratio = (supply > 0) ? demand / supply : 10.0f;
        return it->second.basePrice * std::max(0.1f, std::min(10.0f, ratio));
    }

private:
    std::unordered_map<ResourceType, ResourceDef> resources;
};
} // namespace ogb
