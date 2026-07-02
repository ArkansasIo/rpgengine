/* OGame Beyond - AAIController
 * Developer: Stephen
 * AI controller with behavior tree and utility AI support.
 */
#pragma once
#include "AController.h"

namespace ogb {
class UBehaviorTree;

struct AIBlackboard {
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, bool> bools;
    std::unordered_map<std::string, std::string> strings;

    void SetFloat(const std::string& k, float v) { floats[k] = v; }
    float GetFloat(const std::string& k, float d = 0) const {
        auto it = floats.find(k); return it != floats.end() ? it->second : d;
    }
    void SetInt(const std::string& k, int v) { ints[k] = v; }
    int GetInt(const std::string& k, int d = 0) const {
        auto it = ints.find(k); return it != ints.end() ? it->second : d;
    }
    void SetBool(const std::string& k, bool v) { bools[k] = v; }
    bool GetBool(const std::string& k, bool d = false) const {
        auto it = bools.find(k); return it != bools.end() ? it->second : d;
    }
    void SetString(const std::string& k, const std::string& v) { strings[k] = v; }
    std::string GetString(const std::string& k, const std::string& d = "") const {
        auto it = strings.find(k); return it != strings.end() ? it->second : d;
    }
};

class AAIController : public AController {
public:
    AAIController() { SetName("AIController"); }
    const char* GetClassName() const override { return "AAIController"; }

    AIBlackboard& GetBlackboard() { return blackboard; }

    void SetBehaviorTree(UBehaviorTree* bt) { behaviorTree = bt; }
    UBehaviorTree* GetBehaviorTree() const { return behaviorTree; }

    // AI state
    enum class AIBehavior { Idle, Patrol, Attack, Flee, Gather, Build, Research };
    AIBehavior currentBehavior = AIBehavior::Idle;
    float thinkTimer = 0;
    float thinkInterval = 0.5f;

    void UpdateBehavior(float dt) {
        thinkTimer += dt;
        if (thinkTimer >= thinkInterval) {
            thinkTimer = 0;
            EvaluateBehavior();
        }
    }

    virtual void EvaluateBehavior() {}

protected:
    UBehaviorTree* behaviorTree = nullptr;
    AIBlackboard blackboard;
};
} // namespace ogb
