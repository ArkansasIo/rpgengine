/* ArkLight Beyond - ENet Networking
 * Developer: Stephen
 * Reliable UDP networking for MMO and RTS multiplayer.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <unordered_map>

namespace ogb {

enum class NetRole { Authority, SimulatedProxy, AutonomousProxy };
enum class NetMode { Standalone, ListenServer, DedicatedServer, Client };
enum class ConnectionState { Disconnected, Connecting, Connected, Disconnecting };

struct NetworkPacket {
    uint16_t channelID = 0;
    uint32_t sequenceID = 0;
    uint8_t reliability = 2; // 0=unreliable, 1=unsequenced, 2=reliable
    std::vector<uint8_t> payload;
    double timestamp = 0;
};

struct ENetPeer {
    uint32_t peerID = 0;
    std::string address;
    uint16_t port = 0;
    ConnectionState state = ConnectionState::Disconnected;
    float latency = 0;
    float packetLoss = 0;
    uint32_t packetsSent = 0;
    uint32_t packetsReceived = 0;
    uint32_t bytesSent = 0;
    uint32_t bytesReceived = 0;
    bool isHost = false;
};

struct ReplicationState {
    uint32_t objectID = 0;
    uint32_t version = 0;
    bool isRelevant = true;
    bool isReplicating = true;
    float priority = 1.0f;
    NetRole role = NetRole::SimulatedProxy;
};

struct ChannelConfig {
    std::string name;
    uint8_t reliability = 2; // 0=unreliable, 1=unsequenced, 2=reliable
    uint32_t priority = 0;
    bool isOrdered = true;
};

class ENetNetworkManager {
public:
    NetMode netMode = NetMode::Standalone;
    int maxPeers = 64;
    uint16_t port = 7777;

    void Init(NetMode mode, uint16_t p = 7777) {
        netMode = mode;
        port = p;
        // Initialize ENet
        // Create host or connect
    }

    void Shutdown() {
        // Disconnect all peers
        // Cleanup ENet
    }

    void Update(float dt) {
        // Process events
        // Send/receive packets
        // Replicate state
    }

    // Connection management
    bool Host(int maxConnections = 64) {
        netMode = NetMode::DedicatedServer;
        return true;
    }

    bool Connect(const std::string& address, uint16_t port) {
        ENetPeer peer;
        peer.address = address;
        peer.port = port;
        peer.state = ConnectionState::Connecting;
        peers.push_back(peer);
        return true;
    }

    void DisconnectPeer(uint32_t peerID) {
        for (auto& p : peers) {
            if (p.peerID == peerID) { p.state = ConnectionState::Disconnecting; return; }
        }
    }

    void DisconnectAll() {
        for (auto& p : peers) p.state = ConnectionState::Disconnecting;
    }

    // Packet sending
    void SendPacket(uint32_t peerID, const NetworkPacket& packet) {
        for (auto& p : peers) {
            if (p.peerID == peerID && p.state == ConnectionState::Connected) {
                p.packetsSent++;
                p.bytesSent += static_cast<uint32_t>(packet.payload.size());
            }
        }
    }

    void BroadcastPacket(const NetworkPacket& packet) {
        for (auto& p : peers) {
            if (p.state == ConnectionState::Connected) {
                p.packetsSent++;
                p.bytesSent += static_cast<uint32_t>(packet.payload.size());
            }
        }
    }

    void BroadcastPacketOnChannel(uint8_t channel, const NetworkPacket& packet) {
        BroadcastPacket(packet);
    }

    // Channel management
    void CreateChannel(const std::string& name, uint8_t reliability = 2) {
        ChannelConfig ch;
        ch.name = name;
        ch.reliability = reliability;
        ch.priority = static_cast<uint32_t>(channels.size());
        channels.push_back(ch);
    }

    // Access
    ENetPeer* GetPeer(uint32_t id) {
        for (auto& p : peers) if (p.peerID == id) return &p;
        return nullptr;
    }

    std::vector<ENetPeer>& GetPeers() { return peers; }
    int GetPeerCount() const { return static_cast<int>(peers.size()); }
    int GetConnectedCount() const {
        int c = 0; for (auto& p : peers) if (p.state == ConnectionState::Connected) c++;
        return c;
    }

    bool IsServer() const { return netMode == NetMode::DedicatedServer || netMode == NetMode::ListenServer; }
    bool IsClient() const { return netMode == NetMode::Client; }

    // Bandwidth monitoring
    float GetIncomingBandwidth() const { float total = 0; for (auto& p : peers) total += p.bytesReceived; return total; }
    float GetOutgoingBandwidth() const { float total = 0; for (auto& p : peers) total += p.bytesSent; return total; }

    // Events
    std::function<void(uint32_t)> onPeerConnected;
    std::function<void(uint32_t)> onPeerDisconnected;
    std::function<void(uint32_t, const NetworkPacket&)> onPacketReceived;
    std::function<void(uint32_t, float)> onLatencyUpdate;

private:
    std::vector<ENetPeer> peers;
    std::vector<ChannelConfig> channels;
    uint32_t nextPeerID = 1;
};

} // namespace ogb
