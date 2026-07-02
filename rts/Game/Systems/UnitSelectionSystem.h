/* ArcLight Engine - Unit Selection System
 * Developer: Stephen
 * Handles marker clicks selecting actual units, camera focus,
 * and selection box mechanics for RTS gameplay.
 */

#pragma once

#include <vector>
#include <functional>
#include <string>
#include <algorithm>
#include <cmath>

namespace arclight {

struct SelectableUnit {
	std::string id;
	std::string name;
	std::string typeName;
	float3 position = ZeroVector;
	float3 velocity = ZeroVector;
	float health = 100.0f;
	float maxHealth = 100.0f;
	float shield = 0.0f;
	float maxShield = 0.0f;
	int ownerTeam = -1;
	float selectionRadius = 20.0f;
	bool isSelected = false;
	bool isVisible = true;
	bool isAlive = true;

	// Command state
	std::string currentCommand;
	std::string commandTarget;
	float3 commandTargetPos = ZeroVector;
};

struct SelectionBox {
	float3 topLeft = ZeroVector;
	float3 bottomRight = ZeroVector;
	bool isActive = false;
};

struct CameraTarget {
	float3 position = ZeroVector;
	float zoom = 1.0f;
	float yaw = 0.0f;
	float pitch = -45.0f;
	float transitionSpeed = 5.0f;
	bool isTransitioning = false;
	float transitionProgress = 0.0f;
	float3 startPosition = ZeroVector;
	float3 targetPosition = ZeroVector;
};

class UnitSelectionSystem {
public:
	std::vector<SelectableUnit> units;
	std::vector<SelectableUnit*> selectedUnits;
	SelectionBox selectionBox;
	CameraTarget cameraTarget;
	std::string activeCommand;
	float3 commandTargetPos = ZeroVector;
	bool isCommandMode = false;

	// Selection limits
	int maxSelection = 100;
	bool dragSelectEnabled = true;

	void Update(float dt) {
		// Update camera transition
		if (cameraTarget.isTransitioning) {
			cameraTarget.transitionProgress += dt * cameraTarget.transitionSpeed;
			if (cameraTarget.transitionProgress >= 1.0f) {
				cameraTarget.transitionProgress = 1.0f;
				cameraTarget.isTransitioning = false;
			}
		}

		// Update selection box
		if (selectionBox.isActive) {
			// Selection box is being dragged
		}

		// Clean up dead unit selections
		selectedUnits.erase(
			std::remove_if(selectedUnits.begin(), selectedUnits.end(),
				[](const SelectableUnit* u) { return !u->isAlive; }),
			selectedUnits.end()
		);
	}

	// Click on a map marker/unit - selects the closest unit
	SelectableUnit* ClickAtPosition(const float3& worldPos, float maxDistance = 50.0f) {
		SelectableUnit* closest = nullptr;
		float closestDist = maxDistance;

		for (auto& unit : units) {
			if (!unit.isVisible || !unit.isAlive) continue;
			float dist = (unit.position - worldPos).Length();
			if (dist < closestDist) {
				closestDist = dist;
				closest = &unit;
			}
		}

		if (closest) {
			SelectUnit(closest);
			return closest;
		}
		return nullptr;
	}

	// Double-click to focus camera on unit
	SelectableUnit* DoubleClickFocus(const float3& worldPos, float maxDistance = 50.0f) {
		SelectableUnit* unit = ClickAtPosition(worldPos, maxDistance);
		if (unit) {
			FocusCamera(unit->position);
		}
		return unit;
	}

	// Select a single unit (clears previous selection unless shift held)
	void SelectUnit(SelectableUnit* unit, bool addToSelection = false) {
		if (!addToSelection) {
			ClearSelection();
		}
		if (unit && !unit->isSelected) {
			unit->isSelected = true;
			selectedUnits.push_back(unit);
		}
	}

	// Select units in a rectangular area
	void SelectInBox(const float3& boxMin, const float3& boxMax) {
		ClearSelection();
		for (auto& unit : units) {
			if (!unit.isVisible || !unit.isAlive) continue;
			if (unit.position.x >= boxMin.x && unit.position.x <= boxMax.x &&
				unit.position.z >= boxMin.z && unit.position.z <= boxMax.z) {
				unit.isSelected = true;
				selectedUnits.push_back(&unit);
				if (static_cast<int>(selectedUnits.size()) >= maxSelection) break;
			}
		}
	}

	// Select all units of a type
	void SelectByType(const std::string& typeName) {
		ClearSelection();
		for (auto& unit : units) {
			if (!unit.isVisible || !unit.isAlive) continue;
			if (unit.typeName == typeName) {
				unit.isSelected = true;
				selectedUnits.push_back(&unit);
				if (static_cast<int>(selectedUnits.size()) >= maxSelection) break;
			}
		}
	}

	// Select all units of a team
	void SelectByTeam(int teamID) {
		ClearSelection();
		for (auto& unit : units) {
			if (!unit.isVisible || !unit.isAlive) continue;
			if (unit.ownerTeam == teamID) {
				unit.isSelected = true;
				selectedUnits.push_back(&unit);
				if (static_cast<int>(selectedUnits.size()) >= maxSelection) break;
			}
		}
	}

	// Clear all selections
	void ClearSelection() {
		for (auto* u : selectedUnits) u->isSelected = false;
		selectedUnits.clear();
	}

	// Get center of selected units
	float3 GetSelectionCenter() const {
		if (selectedUnits.empty()) return ZeroVector;
		float3 center = ZeroVector;
		for (auto* u : selectedUnits) center += u->position;
		return center * (1.0f / selectedUnits.size());
	}

	// Get average health of selection
	float GetSelectionHealthPercent() const {
		if (selectedUnits.empty()) return 0.0f;
		float total = 0;
		for (auto* u : selectedUnits) total += u->health / u->maxHealth;
		return total / selectedUnits.size();
	}

	// Camera focus on a position
	void FocusCamera(const float3& pos) {
		cameraTarget.startPosition = cameraTarget.position;
		cameraTarget.targetPosition = pos;
		cameraTarget.isTransitioning = true;
		cameraTarget.transitionProgress = 0.0f;
	}

	// Focus on selected units
	void FocusOnSelection() {
		if (!selectedUnits.empty()) {
			FocusCamera(GetSelectionCenter());
		}
	}

	// Issue command to selected units
	void IssueCommand(const std::string& command, const float3& targetPos) {
		activeCommand = command;
		commandTargetPos = targetPos;
		isCommandMode = true;

		for (auto* u : selectedUnits) {
			u->currentCommand = command;
			u->commandTargetPos = targetPos;
		}

		if (onCommandIssued) onCommandIssued(command, targetPos, selectedUnits);
	}

	// Issue attack command
	void IssueAttackCommand(SelectableUnit* target) {
		if (!target || selectedUnits.empty()) return;
		for (auto* u : selectedUnits) {
			u->currentCommand = "attack";
			u->commandTarget = target->id;
			u->commandTargetPos = target->position;
		}
		if (onAttackCommand) onAttackCommand(selectedUnits, target);
	}

	// Issue move command
	void IssueMoveCommand(const float3& targetPos) {
		IssueCommand("move", targetPos);
	}

	// Issue stop command
	void IssueStopCommand() {
		for (auto* u : selectedUnits) {
			u->currentCommand = "stop";
			u->commandTargetPos = u->position;
		}
		if (onStopCommand) onStopCommand(selectedUnits);
	}

	// Issue hold position command
	void IssueHoldPosition() {
		for (auto* u : selectedUnits) {
			u->currentCommand = "hold";
			u->commandTargetPos = u->position;
		}
	}

	// Issue patrol command
	void IssuePatrolCommand(const float3& targetPos) {
		IssueCommand("patrol", targetPos);
	}

	// Issue guard command
	void IssueGuardCommand(SelectableUnit* guardTarget) {
		if (!guardTarget) return;
		for (auto* u : selectedUnits) {
			u->currentCommand = "guard";
			u->commandTarget = guardTarget->id;
		}
	}

	// Start drag selection
	void StartDragSelect(const float3& start) {
		selectionBox.topLeft = start;
		selectionBox.bottomRight = start;
		selectionBox.isActive = true;
	}

	// Update drag selection
	void UpdateDragSelect(const float3& current) {
		selectionBox.bottomRight = current;
	}

	// End drag selection and select units in box
	void EndDragSelect() {
		if (selectionBox.isActive) {
			float3 boxMin(
				std::min(selectionBox.topLeft.x, selectionBox.bottomRight.x),
				0,
				std::min(selectionBox.topLeft.z, selectionBox.bottomRight.z)
			);
			float3 boxMax(
				std::max(selectionBox.topLeft.x, selectionBox.bottomRight.x),
				0,
				std::max(selectionBox.topLeft.z, selectionBox.bottomRight.z)
			);
			SelectInBox(boxMin, boxMax);
			selectionBox.isActive = false;
		}
	}

	// Get selection info for UI display
	struct SelectionInfo {
		int count = 0;
		std::string primaryType;
		float totalHealth = 0;
		float totalMaxHealth = 0;
		bool allSameType = true;
		std::vector<std::string> abilities;
	};

	SelectionInfo GetSelectionInfo() const {
		SelectionInfo info;
		info.count = static_cast<int>(selectedUnits.size());
		if (selectedUnits.empty()) return info;

		info.primaryType = selectedUnits[0]->typeName;
		for (auto* u : selectedUnits) {
			info.totalHealth += u->health;
			info.totalMaxHealth += u->maxHealth;
			if (u->typeName != info.primaryType) info.allSameType = false;
		}
		return info;
	}

	// Callbacks
	std::function<void(const std::string&, const float3&, const std::vector<SelectableUnit*>&)> onCommandIssued;
	std::function<void(const std::vector<SelectableUnit*>&, SelectableUnit*)> onAttackCommand;
	std::function<void(const std::vector<SelectableUnit*>&)> onStopCommand;
	std::function<void(const SelectableUnit&)> onUnitSelected;
};

} // namespace arclight
