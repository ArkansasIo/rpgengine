/* OGame Beyond - Fleet System
 * Developer: Stephen
 * Fleet management, orders, and movement.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <vector>
#include <string>

namespace ogb {
enum class FleetOrder { Move, Attack, Patrol, Guard, Retreat, Explore, Raid, Siege, Hold, Disband };
enum class FleetState { Idle, Moving, Engaging, Retreating, Disbanded };

struct Fleet {
    int id = -1;
    std::string name;
    int ownerEmpireID = -1;
    std::vector<int> shipIDs;
    Vec3 position;
    Vec3 destination;
    FleetOrder currentOrder = FleetOrder::Idle;
    FleetState state = FleetState::Idle;
    float morale = 100;
    float supplyLevel = 100;
    bool isSelected = false;

    int GetShipCount() const { return static_cast<int>(shipIDs.size()); }
    float GetTotalFirepower() const { return static_cast<float>(shipIDs.size()) * 10.0f; }
    float GetAverageSpeed() const { return shipIDs.empty() ? 0 : 15.0f; }
    bool IsAlive() const { return !shipIDs.empty(); }
};

class FleetManager {
public:
    std::vector<Fleet> fleets;
    int nextFleetID = 1;

    Fleet& CreateFleet(const std::string& name, int empireID) {
        Fleet f;
        f.id = nextFleetID++;
        f.name = name;
        f.ownerEmpireID = empireID;
        fleets.push_back(f);
        return fleets.back();
    }

    Fleet* GetFleet(int id) {
        for (auto& f : fleets) if (f.id == id) return &f;
        return nullptr;
    }

    std::vector<Fleet*> GetFleetsForEmpire(int empireID) {
        std::vector<Fleet*> result;
        for (auto& f : fleets) if (f.ownerEmpireID == empireID) result.push_back(&f);
        return result;
    }

    void Update(float dt) {
        for (auto& f : fleets) {
            if (f.state == FleetState::Moving) {
                Vec3 dir = f.destination - f.position;
                float dist = dir.Length();
                if (dist > 1.0f) {
                    f.position += dir.Normalized() * f.GetAverageSpeed() * dt;
                } else {
                    f.state = FleetState::Idle;
                    f.currentOrder = FleetOrder::Hold;
                }
            }
        }
    }
};
} // namespace ogb
