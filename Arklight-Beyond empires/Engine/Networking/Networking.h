/* ArkLight Beyond - Networking
 * Developer: Stephen
 * Client-server networking with replication and MMO support.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ogb {
enum class NetRole { Authority, SimulatedProxy, AutonomousProxy };
enum class NetMode { Standalone, ListenServer, DedicatedServer, Client };

struct ReplicationInfo {
    uint32_t objectID = 0;
    uint32_t version = 0;
    bool isRelevant = true;
    bool isReplicating = false;
};

struct NetworkPacket {
    uint16_t type = 0;
    uint32_t sequence = 0;
    std::vector<uint8_t> data;
    double timestamp = 0;
};

class NetConnection {
public:
    std::string address;
    int port = 0;
    bool isConnected = false;
    float latency = 0;
    float packetLoss = 0;

    bool Connect(const std::string& addr, int p) { address = addr; port = p; isConnected = true; return true; }
    void Disconnect() { isConnected = false; }
    bool Send(const NetworkPacket& packet) { return isConnected; }
};

class NetworkManager {
public:
    NetMode netMode = NetMode::Standalone;
    int maxConnections = 100;

    void Init(NetMode mode) { netMode = mode; }

    void Update(float dt) {
        // Process incoming packets
        // Replicate state
        // Check connection health
    }

    bool IsServer() const { return netMode == NetMode::DedicatedServer || netMode == NetMode::ListenServer; }
    bool IsClient() const { return netMode == NetMode::Client; }
    bool IsStandalone() const { return netMode == NetMode::Standalone; }

    void Broadcast(const NetworkPacket& packet) {
        for (auto& conn : connections) {
            if (conn.isConnected) conn.Send(packet);
        }
    }

    void SendTo(int connectionID, const NetworkPacket& packet) {
        if (connectionID >= 0 && connectionID < static_cast<int>(connections.size())) {
            connections[connectionID].Send(packet);
        }
    }

    std::vector<NetConnection>& GetConnections() { return connections; }
    int GetConnectionCount() const { return static_cast<int>(connections.size()); }

    std::function<void(int, const NetworkPacket&)> onPacketReceived;

private:
    std::vector<NetConnection> connections;
};
} // namespace ogb
