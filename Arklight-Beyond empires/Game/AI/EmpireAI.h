/* OGame Beyond - AI Empire System
 * Developer: Stephen
 * AI-driven empire management with economy, military, and diplomacy.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ogb {

enum class EAIPersonality { Aggressive, Balanced, Economic, Defensive, Explorer };
enum class EAIGoal { Expand, Conquer, Research, Trade, Fortify, Explore, Diplomacy };

struct AIPersonalityTraits {
    EAIPersonality personality = EAIPersonality::Balanced;
    float aggressionLevel = 0.5f;
    float economicFocus = 0.5f;
    float researchFocus = 0.5f;
    float militaryFocus = 0.5f;
    float diplomacyFocus = 0.5f;
    float explorationFocus = 0.3f;
    bool isHostile = false;
    bool prefersAlliances = false;
    bool rushesMilitary = false;
    bool turtles = false;
};

struct AIDecision {
    EAIGoal goal = EAIGoal::Expand;
    float priority = 0;
    std::string targetID;
    float3 targetPosition;
    std::unordered_map<std::string, float> parameters;
};

class EmpireAI {
public:
    int empireID = -1;
    std::string name;
    AIPersonalityTraits traits;
    std::vector<AIDecision> pendingDecisions;
    std::unordered_map<std::string, float> blackboard;

    float thinkTimer = 0;
    float thinkInterval = 1.0f;
    bool isActive = true;

    void Init(int id, const std::string& aiName, EAIPersonality personality) {
        empireID = id;
        name = aiName;
        traits.personality = personality;
        switch (personality) {
            case EAIPersonality::Aggressive:
                traits.aggressionLevel = 0.9f; traits.militaryFocus = 0.8f;
                break;
            case EAIPersonality::Economic:
                traits.economicFocus = 0.9f; traits.researchFocus = 0.7f;
                break;
            case EAIPersonality::Defensive:
                traits.defensiveFocus = 0.8f; traits.militaryFocus = 0.3f;
                break;
            case EAIPersonality::Explorer:
                traits.explorationFocus = 0.9f; traits.aggressionLevel = 0.2f;
                break;
            default:
                break;
        }
    }

    void Update(float dt) {
        if (!isActive) return;
        thinkTimer += dt;
        if (thinkTimer >= thinkInterval) {
            thinkTimer = 0;
            Think();
        }
    }

    virtual void Think() {
        // Evaluate current situation
        // Pick highest priority goal
        // Generate decision
    }

    void SetBlackboardValue(const std::string& key, float value) { blackboard[key] = value; }
    float GetBlackboardValue(const std::string& key, float def = 0) const {
        auto it = blackboard.find(key); return it != blackboard.end() ? it->second : def;
    }

    void QueueDecision(const AIDecision& decision) { pendingDecisions.push_back(decision); }

    AIDecision GetNextDecision() {
        if (pendingDecisions.empty()) return {};
        AIDecision d = pendingDecisions.front();
        pendingDecisions.erase(pendingDecisions.begin());
        return d;
    }

private:
    float defensibleFocus = 0.5f;
};

} // namespace ogb
