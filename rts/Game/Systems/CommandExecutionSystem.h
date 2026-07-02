/* ArcLight Engine - Command Execution System
 * Developer: Stephen
 * Handles RTS command execution, order templates, and unit orders.
 * Wires bottom tab categories into real command execution.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>

namespace arclight {

enum class ECommandType {
	Move,
	Attack,
	Stop,
	Hold,
	Patrol,
	Guard,
	Build,
	Repair,
	Reclaim,
	Research,
	Ability,
	Custom,
};

enum class ECommandState {
	Pending,
	Executing,
	Completed,
	Failed,
	Cancelled,
};

struct Command {
	std::string id;
	ECommandType type = ECommandType::Move;
	ECommandState state = ECommandState::Pending;
	std::string sourceUnitID;
	std::string targetUnitID;
	float3 targetPosition = ZeroVector;
	float3 targetDirection = ZeroVector;
	float priority = 0;
	float timeout = 30.0f;
	float elapsed = 0;
	std::unordered_map<std::string, float> params;
};

struct OrderTemplate {
	std::string name;
	std::string category;
	std::string icon;
	std::string tooltip;
	ECommandType commandType = ECommandType::Move;
	std::unordered_map<std::string, float> defaultParams;
	bool requiresTarget = true;
	bool requiresUnitTarget = false;
	std::function<void(const Command&)> executeFunc;
};

class CommandExecutionSystem {
public:
	std::vector<Command> activeCommands;
	std::vector<OrderTemplate> orderTemplates;
	std::unordered_map<std::string, OrderTemplate> templateRegistry;

	// Command queue per unit
	std::unordered_map<std::string, std::vector<Command>> unitCommandQueues;

	void Init() {
		// Register default order templates
		RegisterTemplate({"move", "Movement", "↗", "Move to location", ECommandType::Move, {}, true, false, nullptr});
		RegisterTemplate({"attack", "Combat", "⚔", "Attack target", ECommandType::Attack, {}, true, true, nullptr});
		RegisterTemplate({"stop", "Control", "⏹", "Stop all orders", ECommandType::Stop, {}, false, false, nullptr});
		RegisterTemplate({"hold", "Control", "🛡", "Hold position", ECommandType::Hold, {}, false, false, nullptr});
		RegisterTemplate({"patrol", "Movement", "🔄", "Patrol between points", ECommandType::Patrol, {}, true, false, nullptr});
		RegisterTemplate({"guard", "Control", "👁", "Guard target unit", ECommandType::Guard, {}, false, true, nullptr});
		RegisterTemplate({"repair", "Utility", "🔧", "Repair target", ECommandType::Repair, {}, false, true, nullptr});
		RegisterTemplate({"reclaim", "Utility", "♻", "Reclaim wreckage", ECommandType::Reclaim, {}, false, true, nullptr});
		RegisterTemplate({"build", "Construction", "🏗", "Build structure", ECommandType::Build, {}, true, false, nullptr});
		RegisterTemplate({"research", "Technology", "🔬", "Research technology", ECommandType::Research, {}, false, false, nullptr});
	}

	void RegisterTemplate(const OrderTemplate& tmpl) {
		templateRegistry[tmpl.name] = tmpl;
		orderTemplates.push_back(tmpl);
	}

	// Create and queue a command
	Command CreateCommand(const std::string& templateName, const std::string& sourceUnit) {
		Command cmd;
		auto it = templateRegistry.find(templateName);
		if (it != templateRegistry.end()) {
			cmd.id = "cmd_" + std::to_string(nextCmdID++);
			cmd.type = it->second.commandType;
			cmd.sourceUnitID = sourceUnit;
			cmd.params = it->second.defaultParams;
			cmd.state = ECommandState::Pending;
		}
		return cmd;
	}

	// Queue a command for a unit
	void QueueCommand(const std::string& unitID, const Command& cmd) {
		unitCommandQueues[unitID].push_back(cmd);
	}

	// Execute the next command in a unit's queue
	void ExecuteNextCommand(const std::string& unitID) {
		auto it = unitCommandQueues.find(unitID);
		if (it == unitCommandQueues.end() || it->second.empty()) return;

		Command& cmd = it->second.front();
		if (cmd.state == ECommandState::Pending) {
			cmd.state = ECommandState::Executing;
			if (onCommandStarted) onCommandStarted(cmd);
		}
	}

	// Complete current command for a unit
	void CompleteCommand(const std::string& unitID) {
		auto it = unitCommandQueues.find(unitID);
		if (it == unitCommandQueues.end() || it->second.empty()) return;

		Command& cmd = it->second.front();
		cmd.state = ECommandState::Completed;
		if (onCommandCompleted) onCommandCompleted(cmd);
		it->second.erase(it->second.begin());
	}

	// Cancel all commands for a unit
	void CancelAllCommands(const std::string& unitID) {
		auto it = unitCommandQueues.find(unitID);
		if (it != unitCommandQueues.end()) {
			for (auto& cmd : it->second) {
				cmd.state = ECommandState::Cancelled;
				if (onCommandCancelled) onCommandCancelled(cmd);
			}
			it->second.clear();
		}
	}

	// Update command timeouts
	void Update(float dt) {
		for (auto& [unitID, queue] : unitCommandQueues) {
			for (auto& cmd : queue) {
				if (cmd.state == ECommandState::Executing) {
					cmd.elapsed += dt;
					if (cmd.timeout > 0 && cmd.elapsed >= cmd.timeout) {
						cmd.state = ECommandState::Failed;
						if (onCommandFailed) onCommandFailed(cmd);
					}
				}
			}
			// Remove completed/failed/cancelled commands
			queue.erase(
				std::remove_if(queue.begin(), queue.end(),
					[](const Command& c) {
						return c.state == ECommandState::Completed ||
						       c.state == ECommandState::Failed ||
						       c.state == ECommandState::Cancelled;
					}),
				queue.end()
			);
		}
	}

	// Get command queue for a unit
	const std::vector<Command>& GetCommandQueue(const std::string& unitID) const {
		static const std::vector<Command> empty;
		auto it = unitCommandQueues.find(unitID);
		return (it != unitCommandQueues.end()) ? it->second : empty;
	}

	// Get all templates for a category
	std::vector<OrderTemplate*> GetTemplatesByCategory(const std::string& category) {
		std::vector<OrderTemplate*> result;
		for (auto& tmpl : orderTemplates) {
			if (tmpl.category == category) result.push_back(&tmpl);
		}
		return result;
	}

	// Get template by name
	OrderTemplate* GetTemplate(const std::string& name) {
		auto it = templateRegistry.find(name);
		return (it != templateRegistry.end()) ? &it->second : nullptr;
	}

	// Get all unique categories
	std::vector<std::string> GetCategories() const {
		std::vector<std::string> cats;
		for (auto& tmpl : orderTemplates) {
			if (std::find(cats.begin(), cats.end(), tmpl.category) == cats.end()) {
				cats.push_back(tmpl.category);
			}
		}
		return cats;
	}

	// Callbacks
	std::function<void(const Command&)> onCommandStarted;
	std::function<void(const Command&)> onCommandCompleted;
	std::function<void(const Command&)> onCommandFailed;
	std::function<void(const Command&)> onCommandCancelled;

private:
	uint32_t nextCmdID = 1;
};

} // namespace arclight
