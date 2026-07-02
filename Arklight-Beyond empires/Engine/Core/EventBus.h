/* OGame Beyond - Event Bus
 * Developer: Stephen
 * Global event system for decoupled communication between systems.
 */
#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>
#include <any>

namespace ogb {
using EventID = uint32_t;

struct EventData {
    EventID id = 0;
    std::any payload;
    Tick timestamp = 0;
};

class EventBus {
public:
    using Handler = std::function<void(const EventData&)>;

    void Subscribe(EventID id, Handler handler) {
        handlers[id].push_back(std::move(handler));
    }

    void Publish(EventID id, std::any payload = {}, Tick time = 0) {
        EventData evt; evt.id = id; evt.payload = std::move(payload); evt.timestamp = time;
        auto it = handlers.find(id);
        if (it != handlers.end()) {
            for (auto& h : it->second) h(evt);
        }
        for (auto& h : globalHandlers) h(evt);
    }

    void SubscribeAll(Handler handler) { globalHandlers.push_back(std::move(handler)); }
    void Clear() { handlers.clear(); globalHandlers.clear(); }

private:
    std::unordered_map<EventID, std::vector<Handler>> handlers;
    std::vector<Handler> globalHandlers;
};

// Common event IDs
namespace Events {
    constexpr EventID UnitCreated = 1;
    constexpr EventID UnitDestroyed = 2;
    constexpr EventID UnitDamaged = 3;
    constexpr EventID BuildingConstructed = 4;
    constexpr EventID BuildingDestroyed = 5;
    constexpr EventID ResearchCompleted = 6;
    constexpr EventID FleetEngaged = 7;
    constexpr EventID FleetDestroyed = 8;
    constexpr EventID DiplomacyChanged = 9;
    constexpr EventID ResourceCollected = 10;
    constexpr EventID TradeCompleted = 11;
    constexpr EventID ColonyEstablished = 12;
    constexpr EventID WarDeclared = 13;
    constexpr EventID PeaceOffered = 14;
    constexpr EventID GameSaved = 15;
    constexpr EventID GameLoaded = 16;
    constexpr EventID NewTurn = 17;
    constexpr EventID Notification = 100;
}
} // namespace ogb
