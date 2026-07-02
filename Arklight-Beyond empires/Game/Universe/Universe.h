/* ArkLight Beyond - Universe
 * Developer: Stephen
 * Top-level container for galaxies, players, and global state.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace ogb {
struct PlayerAccount {
    std::string id;
    std::string name;
    int empireID = -1;
    bool isOnline = false;
    int64_t totalScore = 0;
    int rank = 0;
};

class Universe {
public:
    std::string universeID;
    std::string universeName;
    int maxPlayers = 100;
    int currentPlayers = 0;
    GameDate startDate;
    bool isMOMode = false;

    std::vector<PlayerAccount> players;
    std::unordered_map<int, int> empireToPlayer; // empireID -> playerID

    void Init() { startDate = {2200, 1, 1, 0}; }
    void Update(float dt) {}

    int AddPlayer(const std::string& name) {
        PlayerAccount p;
        p.id = "player_" + std::to_string(players.size());
        p.name = name;
        p.empireID = static_cast<int>(players.size());
        players.push_back(p);
        currentPlayers++;
        return p.empireID;
    }

    PlayerAccount* GetPlayer(int id) {
        for (auto& p : players) if (p.empireID == id) return &p;
        return nullptr;
    }
};
} // namespace ogb
