/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired RTS World subsystem.
 * Manages zones, territories, resource nodes, and spatial queries
 * for large-scale RTS/4X gameplay.
 */

#pragma once

#include "../ECS/Entity.h"
#include "../System/float3.h"
#include <vector>
#include <unordered_map>
#include <functional>

namespace arclight {

enum class ZoneType {
	Territory,     // controlled area
	Contested,     // being fought over
	Neutral,       // unclaimed
	Forbidden,     // cannot enter
	Void,          // empty space (4X)
	Orbital,       // space zone (4X)
};

struct ResourceNode {
	float3 position = ZeroVector;
	float metal = 0.0f;
	float energy = 0.0f;
	float rarity = 1.0f;
	int nodeType = 0; // 0=common, 1=rare, 2=epic, 3=legendary
	bool depleted = false;
	float respawnRate = 0.0f;
	float currentAmount = 100.0f;
	float maxAmount = 100.0f;
};

struct Zone {
	int zoneID = -1;
	ZoneType type = ZoneType::Neutral;
	int controllingTeam = -1;
	float3 center = ZeroVector;
	float radius = 500.0f;
	float defenseMultiplier = 1.0f;
	float resourceOutput = 0.0f;
	bool isHQ = false; // headquarters
	std::vector<int> connectedZones; // graph connections
	std::vector<EntityID> entitiesInZone;
};

struct SupplyLine {
	int fromZone = -1;
	int toZone = -1;
	int controllingTeam = -1;
	float throughput = 1.0f;
	bool isActive = true;
	float length = 0.0f;
};

class RTSWorld {
public:
	// Zone Management
	int CreateZone(const float3& center, float radius, ZoneType type = ZoneType::Neutral) {
		Zone zone;
		zone.zoneID = nextZoneID++;
		zone.center = center;
		zone.radius = radius;
		zone.type = type;
		zones[zone.zoneID] = zone;
		return zone.zoneID;
	}

	Zone* GetZone(int zoneID) {
		auto it = zones.find(zoneID);
		return it != zones.end() ? &it->second : nullptr;
	}

	void ConnectZones(int zoneA, int zoneB) {
		zones[zoneA].connectedZones.push_back(zoneB);
		zones[zoneB].connectedZones.push_back(zoneA);
	}

	void CaptureZone(int zoneID, int teamID) {
		auto& zone = zones[zoneID];
		zone.controllingTeam = teamID;
		zone.type = ZoneType::Territory;
		for (int connected : zone.connectedZones) {
			if (zones[connected].controllingTeam == teamID) {
				// Create supply line
				SupplyLine line;
				line.fromZone = zoneID;
				line.toZone = connected;
				line.controllingTeam = teamID;
				line.length = (zone.center - zones[connected].center).Length();
				supplyLines.push_back(line);
			}
		}
	}

	// Resource Management
	int CreateResourceNode(const float3& position, float amount, int type = 0) {
		ResourceNode node;
		node.position = position;
		node.currentAmount = amount;
		node.maxAmount = amount;
		node.nodeType = type;
		node.rarity = 1.0f + type * 0.5f;
		resourceNodes.push_back(node);
		return static_cast<int>(resourceNodes.size()) - 1;
	}

	ResourceNode* GetNearestResourceNode(const float3& position, float maxRange = 5000.0f) {
		ResourceNode* nearest = nullptr;
		float nearestDist = maxRange;
		for (auto& node : resourceNodes) {
			if (node.depleted) continue;
			float dist = (node.position - position).Length();
			if (dist < nearestDist) {
				nearestDist = dist;
				nearest = &node;
			}
		}
		return nearest;
	}

	void UpdateResources(float dt) {
		for (auto& node : resourceNodes) {
			if (node.depleted) continue;
			if (node.respawnRate > 0.0f && node.currentAmount < node.maxAmount) {
				node.currentAmount = std::min(node.maxAmount, node.currentAmount + node.respawnRate * dt);
			}
		}
	}

	// Team economy per tick
	float CalculateTeamIncome(int teamID) const {
		float totalIncome = 0.0f;
		for (auto& [id, zone] : zones) {
			if (zone.controllingTeam == teamID) {
				totalIncome += zone.resourceOutput;
			}
		}
		for (auto& line : supplyLines) {
			if (line.controllingTeam == teamID && line.isActive) {
				totalIncome *= line.throughput;
			}
		}
		return totalIncome;
	}

	int GetTeamZoneCount(int teamID) const {
		int count = 0;
		for (auto& [id, zone] : zones) {
			if (zone.controllingTeam == teamID) count++;
		}
		return count;
	}

	// Spatial queries
	std::vector<EntityID> GetEntitiesInRadius(const float3& center, float radius, int teamID = -1) {
		std::vector<EntityID> result;
		for (auto& [id, zone] : zones) {
			if (teamID >= 0 && zone.controllingTeam != teamID) continue;
			float dist = (zone.center - center).Length();
			if (dist <= radius + zone.radius) {
				result.insert(result.end(), zone.entitiesInZone.begin(), zone.entitiesInZone.end());
			}
		}
		return result;
	}

	// Pathfinding between zones (BFS)
	std::vector<int> FindPath(int fromZone, int toZone) {
		if (fromZone == toZone) return {fromZone};

		std::unordered_map<int, int> cameFrom;
		std::vector<int> queue = {fromZone};
		cameFrom[fromZone] = -1;

		while (!queue.empty()) {
			int current = queue.front();
			queue.erase(queue.begin());

			if (current == toZone) {
				// Reconstruct path
				std::vector<int> path;
				int node = toZone;
				while (node != -1) {
					path.insert(path.begin(), node);
					node = cameFrom[node];
				}
				return path;
			}

			for (int neighbor : zones[current].connectedZones) {
				if (cameFrom.find(neighbor) == cameFrom.end()) {
					cameFrom[neighbor] = current;
					queue.push_back(neighbor);
				}
			}
		}

		return {}; // no path found
	}

	// Get control percentage for a team
	float GetTeamControlPercent(int teamID) const {
		if (zones.empty()) return 0.0f;
		int teamZones = 0;
		for (auto& [id, zone] : zones) {
			if (zone.controllingTeam == teamID) teamZones++;
		}
		return static_cast<float>(teamZones) / zones.size();
	}

	std::unordered_map<int, Zone>& GetZones() { return zones; }
	std::vector<ResourceNode>& GetResourceNodes() { return resourceNodes; }
	std::vector<SupplyLine>& GetSupplyLines() { return supplyLines; }

private:
	int nextZoneID = 0;
	std::unordered_map<int, Zone> zones;
	std::vector<ResourceNode> resourceNodes;
	std::vector<SupplyLine> supplyLines;
};

} // namespace arclight
