/* ArcLight Engine - Key Bindings
 * Developer: Stephen
 * Configurable key/input mapping system with presets.
 */

#pragma once

#include <string>
#include <unordered_map>

namespace arclight {

enum class InputAction {
	// Camera
	CameraMoveForward,
	CameraMoveBackward,
	CameraMoveLeft,
	CameraMoveRight,
	CameraRotateLeft,
	CameraRotateRight,
	CameraZoomIn,
	CameraZoomOut,
	CameraReset,
	CameraFreeLook,

	// Selection
	SelectUnit,
	DeselectAll,
	SelectAllUnits,
	SelectAllUnitsOnScreen,
	SelectAllUnitsOfType,
	SelectBuilder,
	SelectCombatUnit,
	SelectNavalUnit,
	SelectAirUnit,

	// Orders
	OrderMove,
	OrderAttack,
	OrderPatrol,
	OrderGuard,
	OrderStop,
	OrderHoldPosition,
	OrderRetreat,
	OrderRepair,
	OrderReclaim,
	OrderResurrect,

	// Building
	BuildMenu,
	CancelBuild,
	RepeatBuild,

	// Grouping
	GroupSelect1,
	GroupSelect2,
	GroupSelect3,
	GroupSelect4,
	GroupSelect5,
	GroupSelect6,
	GroupSelect7,
	GroupSelect8,
	GroupSelect9,
	GroupSelect0,
	GroupAssign1,
	GroupAssign2,
	GroupAssign3,
	GroupAssign4,
	GroupAssign5,
	GroupAssign6,
	GroupAssign7,
	GroupAssign8,
	GroupAssign9,
	GroupAssign0,

	// UI
	ToggleMinimap,
	ToggleOverview,
	ToggleInfoPanel,
	ToggleTeamColors,
	ToggleLOS,
	ToggleMesaageLog,
	Chat,
	ChatAlliance,
	ChatTeam,

	// Game
	Pause,
	SpeedUp,
	SlowDown,
	NormalSpeed,
	Screenshot,
	ToggleUI,
	Quit,

	MAX_ACTIONS,
};

struct KeyBinding {
	int primaryKey = 0;
	int secondaryKey = 0;
	bool allowModifier = false;
	bool consumeEvent = true;
};

class KeyBindingSystem {
public:
	void SetDefaultBindings() {
		// Camera
		bindings[InputAction::CameraMoveForward] = { 'W', 0 };
		bindings[InputAction::CameraMoveBackward] = { 'S', 0 };
		bindings[InputAction::CameraMoveLeft] = { 'A', 0 };
		bindings[InputAction::CameraMoveRight] = { 'D', 0 };
		bindings[InputAction::CameraRotateLeft] = { 'Q', 0 };
		bindings[InputAction::CameraRotateRight] = { 'E', 0 };
		bindings[InputAction::CameraZoomIn] = { 0, 0 };
		bindings[InputAction::CameraZoomOut] = { 0, 0 };
		bindings[InputAction::CameraReset] = { 'Home', 0 };
		bindings[InputAction::CameraFreeLook] = { 'Ctrl', 'F' };

		// Selection
		bindings[InputAction::SelectUnit] = { 0, 0 }; // Left click
		bindings[InputAction::DeselectAll] = { 'Escape', 0 };
		bindings[InputAction::SelectAllUnits] = { 'Ctrl', 'A' };
		bindings[InputAction::SelectAllUnitsOnScreen] = { 'Ctrl', 'Z' };

		// Orders
		bindings[InputAction::OrderMove] = { 0, 0 }; // Right click
		bindings[InputAction::OrderAttack] = { 0, 0 }; // Right click on enemy
		bindings[InputAction::OrderStop] = { 'S', 0 };
		bindings[InputAction::OrderHoldPosition] = { 'H', 0 };
		bindings[InputAction::OrderPatrol] = { 'P', 0 };
		bindings[InputAction::OrderGuard] = { 'G', 0 };
		bindings[InputAction::OrderRetreat] = { 'R', 0 };
		bindings[InputAction::OrderRepair] = { 'Shift', 'R' };

		// Grouping
		for (int i = 0; i <= 9; i++) {
			bindings[static_cast<InputAction>(static_cast<int>(InputAction::GroupSelect1) + i)] = { '0' + i, 0 };
			bindings[static_cast<InputAction>(static_cast<int>(InputAction::GroupAssign1) + i)] = { 'Ctrl', '0' + i };
		}

		// UI
		bindings[InputAction::ToggleMinimap] = { 'Tab', 0 };
		bindings[InputAction::Chat] = { 'Enter', 0 };
		bindings[InputAction::ChatTeam] = { 'Shift', 'Enter' };

		// Game
		bindings[InputAction::Pause] = { 'Space', 0 };
		bindings[InputAction::SpeedUp] = { 'Insert', 0 };
		bindings[InputAction::SlowDown] = { 'Delete', 0 };
		bindings[InputAction::NormalSpeed] = { 'Numpad0', 0 };
		bindings[InputAction::Screenshot] = { 'PrintScreen', 0 };
		bindings[InputAction::ToggleUI] = { 'F9', 0 };
	}

	void SetBinding(InputAction action, int primaryKey, int secondaryKey = 0) {
		bindings[action] = { primaryKey, secondaryKey };
	}

	KeyBinding GetBinding(InputAction action) const {
		auto it = bindings.find(action);
		return (it != bindings.end()) ? it->second : KeyBinding();
	}

	InputAction FindActionByKey(int key) const {
		for (auto& [action, binding] : bindings) {
			if (binding.primaryKey == key || binding.secondaryKey == key) {
				return action;
			}
		}
		return InputAction::MAX_ACTIONS;
	}

	void ResetToDefaults() {
		bindings.clear();
		SetDefaultBindings();
	}

private:
	std::unordered_map<InputAction, KeyBinding> bindings;
};

} // namespace arclight
