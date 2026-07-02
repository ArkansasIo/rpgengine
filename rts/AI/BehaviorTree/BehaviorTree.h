/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Behavior Tree system for AI decision-making.
 * Nodes: Composite (Selector, Sequence, Parallel),
 *        Decorator (condition checks),
 *        Task (leaf actions).
 */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace arclight {

enum class ENodeResult {
	Success,
	Failure,
	InProgress,
};

enum class ENodeState {
	Uninitialized,
	Running,
	Completed,
	Failed,
};

// Forward
class BehaviorTree;
class BehaviorTreeComponent;

// ======================== Base Node ========================

class BehaviorNode {
public:
	virtual ~BehaviorNode() = default;
	virtual ENodeResult Execute(BehaviorTreeComponent& ownerComp) = 0;
	virtual void OnInitialized(BehaviorTreeComponent& /*ownerComp*/) {}
	virtual void OnNodeActivated(BehaviorTreeComponent& /*ownerComp*/) {}
	virtual void OnNodeDeactivated(BehaviorTreeComponent& /*ownerComp*/, ENodeResult /*result*/) {}

	std::string nodeName;
	ENodeState state = ENodeState::Uninitialized;
	BehaviorNode* parent = nullptr;
};

// ======================== Composites ========================

class CompositeNode : public BehaviorNode {
public:
	void AddChild(std::unique_ptr<BehaviorNode> child) {
		child->parent = this;
		children.push_back(std::move(child));
	}

	std::vector<std::unique_ptr<BehaviorNode>>& GetChildren() { return children; }

protected:
	std::vector<std::unique_ptr<BehaviorNode>> children;
};

/** Executes children left-to-right, succeeds on first Success */
class SelectorNode : public CompositeNode {
public:
	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		for (auto& child : children) {
			ENodeResult result = child->Execute(ownerComp);
			if (result == ENodeResult::InProgress) return ENodeResult::InProgress;
			if (result == ENodeResult::Success) return ENodeResult::Success;
		}
		return ENodeResult::Failure;
	}
};

/** Executes children left-to-right, fails on first Failure */
class SequenceNode : public CompositeNode {
public:
	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		for (auto& child : children) {
			ENodeResult result = child->Execute(ownerComp);
			if (result == ENodeResult::InProgress) return ENodeResult::InProgress;
			if (result == ENodeResult::Failure) return ENodeResult::Failure;
		}
		return ENodeResult::Success;
	}
};

/** Executes all children simultaneously */
class ParallelNode : public CompositeNode {
public:
	int successThreshold = -1; // -1 = all must succeed

	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		int successes = 0;
		int failures = 0;
		bool hasRunning = false;

		for (auto& child : children) {
			ENodeResult result = child->Execute(ownerComp);
			if (result == ENodeResult::Success) successes++;
			else if (result == ENodeResult::Failure) failures++;
			else hasRunning = true;
		}

		int threshold = (successThreshold > 0) ? successThreshold : static_cast<int>(children.size());
		if (successes >= threshold) return ENodeResult::Success;
		if (failures > 0 && !hasRunning) return ENodeResult::Failure;
		return ENodeResult::InProgress;
	}
};

// ======================== Decorators ========================

class DecoratorNode : public BehaviorNode {
public:
	BehaviorNode* child = nullptr;
	void SetChild(std::unique_ptr<BehaviorNode> node) {
		node->parent = this;
		child = node.release();
	}
	virtual ~DecoratorNode() { delete child; }
};

/** Condition check - returns Success if condition is true, Failure otherwise */
class ConditionDecorator : public DecoratorNode {
public:
	std::function<bool()> condition;

	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		if (condition && condition()) {
			return child ? child->Execute(ownerComp) : ENodeResult::Success;
		}
		return ENodeResult::Failure;
	}
};

/** Inverter - flips Success/Failure */
class InverterDecorator : public DecoratorNode {
public:
	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		if (!child) return ENodeResult::Failure;
		ENodeResult result = child->Execute(ownerComp);
		if (result == ENodeResult::Success) return ENodeResult::Failure;
		if (result == ENodeResult::Failure) return ENodeResult::Success;
		return ENodeResult::InProgress;
	}
};

/** Repeater - executes child N times */
class RepeaterDecorator : public DecoratorNode {
public:
	int repeatCount = -1; // -1 = infinite
	int currentCount = 0;

	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		if (!child) return ENodeResult::Failure;
		if (repeatCount > 0 && currentCount >= repeatCount) return ENodeResult::Success;
		currentCount++;
		return child->Execute(ownerComp);
	}
};

// ======================== Tasks (Leaves) ========================

class TaskNode : public BehaviorNode {
public:
	std::function<ENodeResult(BehaviorTreeComponent&)> executeFunc;

	ENodeResult Execute(BehaviorTreeComponent& ownerComp) override {
		if (executeFunc) return executeFunc(ownerComp);
		return ENodeResult::Success;
	}
};

// ======================== Blackboard ========================

class Blackboard {
public:
	void SetValue(const std::string& key, float value) {
		floatValues[key] = value;
	}

	void SetValue(const std::string& key, int value) {
		intValues[key] = value;
	}

	void SetValue(const std::string& key, bool value) {
		boolValues[key] = value;
	}

	void SetValue(const std::string& key, const std::string& value) {
		stringValues[key] = value;
	}

	void SetValueVec3(const std::string& key, float x, float y, float z) {
		vec3Values[key] = {x, y, z};
	}

	float GetFloat(const std::string& key, float defaultVal = 0.0f) const {
		auto it = floatValues.find(key);
		return it != floatValues.end() ? it->second : defaultVal;
	}

	int GetInt(const std::string& key, int defaultVal = 0) const {
		auto it = intValues.find(key);
		return it != intValues.end() ? it->second : defaultVal;
	}

	bool GetBool(const std::string& key, bool defaultVal = false) const {
		auto it = boolValues.find(key);
		return it != boolValues.end() ? it->second : defaultVal;
	}

	std::string GetString(const std::string& key, const std::string& defaultVal = "") const {
		auto it = stringValues.find(key);
		return it != stringValues.end() ? it->second : defaultVal;
	}

	void ClearAll() {
		floatValues.clear();
		intValues.clear();
		boolValues.clear();
		stringValues.clear();
		vec3Values.clear();
	}

private:
	std::unordered_map<std::string, float> floatValues;
	std::unordered_map<std::string, int> intValues;
	std::unordered_map<std::string, bool> boolValues;
	std::unordered_map<std::string, std::string> stringValues;
	std::unordered_map<std::string, std::array<float,3>> vec3Values;
};

// ======================== Behavior Tree ========================

class BehaviorTreeComponent;

class BehaviorTree {
public:
	std::unique_ptr<BehaviorNode> rootNode;
	Blackboard blackboard;

	void Execute(BehaviorTreeComponent& ownerComp) {
		if (rootNode) {
			rootNode->Execute(ownerComp);
		}
	}
};

class BehaviorTreeComponent {
public:
	BehaviorTree* tree = nullptr;
	bool isActive = false;

	void StartTree(BehaviorTree* bt) {
		tree = bt;
		isActive = true;
	}

	void StopTree() {
		isActive = false;
		tree = nullptr;
	}

	void Update(float /*dt*/) {
		if (isActive && tree) {
			tree->Execute(*this);
		}
	}

	Blackboard& GetBlackboard() {
		return tree->blackboard;
	}
};

} // namespace arclight
