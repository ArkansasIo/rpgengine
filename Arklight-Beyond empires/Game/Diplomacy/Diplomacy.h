/* ArkLight Beyond - Diplomacy
 * Developer: Stephen
 * Empire relations, treaties, and war/peace mechanics.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {
enum class DiplomacyState { Hostile, Unfriendly, Neutral, Friendly, Allied, Vassal };
enum class TreatyType { Peace, Trade, Alliance, NonAggression, Research, Federation, Vassalage };

struct Empire {
    int id = -1;
    std::string name;
    std::string government; // Democracy, Oligarchy, Dictatorship, Hive, etc.
    int leaderID = -1;
    int homeSystemID = -1;
    int fleetCount = 0;
    int colonyCount = 0;
    float influence = 50;
    float64_t totalScore = 0;
    Color emblemColor;
    bool isAlive = true;
};

struct Treaty {
    std::string id;
    TreatyType type = TreatyType::Peace;
    int fromEmpire = -1;
    int toEmpire = -1;
    float duration = -1; // -1 = permanent
    float remainingTime = 0;
    bool isActive = true;
};

struct Relation {
    int fromEmpire = -1;
    int toEmpire = -1;
    DiplomacyState state = DiplomacyState::Neutral;
    float opinion = 0; // -100 to 100
    float trust = 50;
    bool atWar = false;
    float warExhaustion = 0;
};

class DiplomacySystem {
public:
    std::vector<Empire> empires;
    std::vector<Treaty> treaties;
    std::vector<Relation> relations;

    int CreateEmpire(const std::string& name, const std::string& gov, int homeSystem) {
        Empire e;
        e.id = static_cast<int>(empires.size());
        e.name = name;
        e.government = gov;
        e.homeSystemID = homeSystem;
        empires.push_back(e);
        return e.id;
    }

    void SetDiplomacy(int from, int to, DiplomacyState state) {
        for (auto& r : relations) {
            if (r.fromEmpire == from && r.toEmpire == to) { r.state = state; return; }
        }
        Relation r; r.fromEmpire = from; r.toEmpire = to; r.state = state;
        relations.push_back(r);
    }

    DiplomacyState GetDiplomacy(int from, int to) const {
        for (auto& r : relations) if (r.fromEmpire == from && r.toEmpire == to) return r.state;
        return DiplomacyState::Neutral;
    }

    bool AreAllied(int a, int b) const { return GetDiplomacy(a, b) >= DiplomacyState::Allied; }
    bool AreAtWar(int a, int b) const {
        for (auto& r : relations) if (r.fromEmpire == a && r.toEmpire == b) return r.atWar;
        return false;
    }

    void DeclareWar(int from, int to) {
        SetDiplomacy(from, to, DiplomacyState::Hostile);
        SetDiplomacy(to, from, DiplomacyState::Hostile);
        for (auto& r : relations) {
            if ((r.fromEmpire == from && r.toEmpire == to) || (r.fromEmpire == to && r.toEmpire == from)) {
                r.atWar = true;
            }
        }
    }
};
} // namespace ogb
