/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Fog of War component for RTS gameplay.
 * Manages visibility states per-team with smooth transitions.
 */

#pragma once

#include "../Entity.h"
#include <vector>
#include <unordered_map>

namespace arclight {

enum class VisibilityState {
	Hidden,      // never seen
	Explored,    // previously seen, not currently visible
	Visible,     // currently visible
};

struct FogOfWarComponent : public IComponent {
	float sightRadius = 500.0f;
	float losRadius = 300.0f; // line of sight inner radius (guaranteed)
	float losAngle = 360.0f; // cone angle for directional LOS
	float losDirection = 0.0f; // facing direction for cone LOS

	// Per-team visibility
	std::unordered_map<int, VisibilityState> teamVisibility;

	// Radar detection (bypasses terrain)
	bool hasRadar = false;
	float radarRange = 1000.0f;
	bool detectCloaked = false;

	// Cloaking
	bool isCloaked = false;
	float cloakRadius = 0.0f; // units within this radius decloak
	float decloakRadius = 150.0f;
	bool canSeeCloaked = false;

	// Stealth
	bool isStealthy = false; // harder to detect, but not invisible

	VisibilityState GetVisibility(int teamID) const {
		auto it = teamVisibility.find(teamID);
		if (it != teamVisibility.end()) return it->second;
		return VisibilityState::Hidden;
	}

	void SetVisibility(int teamID, VisibilityState state) {
		teamVisibility[teamID] = state;
	}

	bool IsVisibleToTeam(int teamID) const {
		return GetVisibility(teamID) == VisibilityState::Visible;
	}

	void UpdateVisibility(int teamID, bool inLineOfSight, float distance) {
		if (inLineOfSight || (hasRadar && distance <= radarRange)) {
			SetVisibility(teamID, VisibilityState::Visible);
		} else if (GetVisibility(teamID) == VisibilityState::Visible) {
			SetVisibility(teamID, VisibilityState::Explored);
		}
	}

	// Check if this entity can see a target
	bool CanSee(const FogOfWarComponent& target, float distance) const {
		if (target.isCloaked && !canSeeCloaked) return false;
		if (isCloaked && target.canSeeCloaked) return true;
		if (target.isStealthy && distance > losRadius * 0.5f) return false;
		return distance <= sightRadius;
	}

	void ClearAllVisibility() {
		teamVisibility.clear();
	}
};

} // namespace arclight
