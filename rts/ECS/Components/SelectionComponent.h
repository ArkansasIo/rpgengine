/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Selection Component for RTS unit management.
 * Handles group assignment, selection states, and UI feedback.
 */

#pragma once

#include "../Entity.h"
#include "Types.h"
#include <vector>
#include <string>

namespace arclight {

enum class SelectionState {
	Unselected,
	Selected,
	Hovered,       // mouse hover
	Locked,        // locked to target
	FormationLead, // formation leader
};

struct SelectionComponent : public IComponent {
	SelectionState state = SelectionState::Unselected;
	int groupID = -1; // control group (1-0)
	bool isControllable = true;
	bool isSelectable = true;

	// Visual feedback
	float selectionRingRadius = 30.0f;
	float3 selectionRingColor = float3(0.0f, 1.0f, 0.0f);
	float healthBarHeight = 15.0f;
	bool showHealthBar = true;
	bool showSelectionRing = true;
	bool showUnitName = false;

	// Multi-select support
	std::vector<EntityID> subSelections; // units selected as part of this unit's group

	// Drag selection box
	static bool IsInSelectionBox(
		const float3& unitPos,
		const float3& boxMin,
		const float3& boxMax
	) {
		return unitPos.x >= boxMin.x && unitPos.x <= boxMax.x &&
		       unitPos.z >= boxMin.z && unitPos.z <= boxMax.z;
	}

	void Select() {
		if (!isSelectable) return;
		state = SelectionState::Selected;
	}

	void Deselect() {
		state = SelectionState::Unselected;
	}

	void Hover() {
		if (state == SelectionState::Unselected) {
			state = SelectionState::Hovered;
		}
	}

	void Unhover() {
		if (state == SelectionState::Hovered) {
			state = SelectionState::Unselected;
		}
	}

	bool IsSelected() const { return state == SelectionState::Selected; }
	bool IsHovered() const { return state == SelectionState::Hovered; }
};

} // namespace arclight
