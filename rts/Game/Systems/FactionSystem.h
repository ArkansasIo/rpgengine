/* ArcLight Engine - Faction System
 * Developer: Stephen
 * Manages factions, reputation, diplomacy, and alliances.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <functional>

namespace arclight {

enum class DiplomacyState {
	Hostile,
	Unfriendly,
	Neutral,
	Friendly,
	Allied,
};

struct FactionDef {
	std::string id;
	std::string name;
	std::string description;
	float3 color = float3(1, 1, 1);
	float3 homePosition = ZeroVector;
	bool isPlayable = false;
	bool isMajorFaction = true;
};

struct ReputationLevel {
	std::string factionID;
	float reputation = 0.0f; // -100 to 100
	DiplomacyState diplomacy = DiplomacyState::Neutral;
	bool isVisible = false;
};

struct DiplomacyRequest {
	std::string fromFaction;
	std::string toFaction;
	DiplomacyState proposedState;
	int duration = -1; // -1 = permanent
};

class FactionSystem {
public:
	void RegisterFaction(const FactionDef& def) {
		factions[def.id] = def;
		for (auto& [otherID, other] : factions) {
			if (otherID != def.id) {
				diplomacy[{def.id, otherID}] = DiplomacyState::Neutral;
				diplomacy[{otherID, def.id}] = DiplomacyState::Neutral;
			}
		}
	}

	void ChangeReputation(const std::string& factionID, float amount) {
		auto& rep = reputations[factionID];
		rep.reputation = std::max(-100.0f, std::min(100.0f, rep.reputation + amount));
		rep.factionID = factionID;
		UpdateDiplomacyState(factionID);
	}

	void SetDiplomacy(const std::string& from, const std::string& to, DiplomacyState state) {
		diplomacy[{from, to}] = state;
		diplomacy[{to, from}] = state;
		if (onDiplomacyChanged) onDiplomacyChanged(from, to, state);
	}

	DiplomacyState GetDiplomacy(const std::string& from, const std::string& to) const {
		auto it = diplomacy.find({from, to});
		return (it != diplomacy.end()) ? it->second : DiplomacyState::Neutral;
	}

	float GetReputation(const std::string& factionID) const {
		auto it = reputations.find(factionID);
		return (it != reputations.end()) ? it->second.reputation : 0.0f;
	}

	bool AreAllied(const std::string& a, const std::string& b) const {
		return GetDiplomacy(a, b) >= DiplomacyState::Allied;
	}

	bool AreHostile(const std::string& a, const std::string& b) const {
		return GetDiplomacy(a, b) <= DiplomacyState::Hostile;
	}

	const FactionDef* GetFaction(const std::string& id) const {
		auto it = factions.find(id);
		return (it != factions.end()) ? &it->second : nullptr;
	}

	std::function<void(const std::string&, const std::string&, DiplomacyState)> onDiplomacyChanged;

private:
	std::unordered_map<std::string, FactionDef> factions;
	std::unordered_map<std::string, ReputationLevel> reputations;
	std::map<std::pair<std::string, std::string>, DiplomacyState> diplomacy;

	void UpdateDiplomacyState(const std::string& factionID) {
		float rep = GetReputation(factionID);
		DiplomacyState newState;
		if (rep <= -75) newState = DiplomacyState::Hostile;
		else if (rep <= -25) newState = DiplomacyState::Unfriendly;
		else if (rep <= 25) newState = DiplomacyState::Neutral;
		else if (rep <= 75) newState = DiplomacyState::Friendly;
		else newState = DiplomacyState::Allied;
		// Update all bilateral relations with this faction
		for (auto& [key, state] : diplomacy) {
			if (key.first == factionID || key.second == factionID) {
				state = newState;
			}
		}
	}
};

} // namespace arclight
