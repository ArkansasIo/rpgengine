/* ArcLight Engine - Quest System
 * Developer: Stephen
 * Handles quests, objectives, rewards, and quest chains.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace arclight {

enum class QuestStatus {
	Locked,
	Available,
	Active,
	Completed,
	Failed,
};

enum class ObjectiveType {
	Kill,
	Gather,
	TalkTo,
	GoTo,
	Build,
	Survive,
	Custom,
};

struct QuestObjective {
	std::string id;
	std::string description;
	ObjectiveType type = ObjectiveType::Kill;
	std::string targetID;
	int requiredCount = 1;
	int currentCount = 0;
	bool isCompleted = false;

	void Progress(int amount = 1) {
		if (isCompleted) return;
		currentCount += amount;
		if (currentCount >= requiredCount) {
			currentCount = requiredCount;
			isCompleted = true;
		}
	}

	float GetProgress() const {
		return requiredCount > 0 ? static_cast<float>(currentCount) / requiredCount : 0.0f;
	}
};

struct QuestReward {
	int xp = 0;
	int credits = 0;
	std::vector<std::string> itemIDs;
	std::vector<std::string> unlockQuestIDs;
};

struct QuestDef {
	std::string id;
	std::string name;
	std::string description;
	std::string giverNPC;
	int levelRequired = 1;
	std::vector<std::string> prerequisiteQuestIDs;
	std::vector<QuestObjective> objectives;
	QuestReward reward;
	bool isRepeatable = false;
	bool isMainQuest = false;
};

class QuestSystem {
public:
	void RegisterQuest(const QuestDef& def) {
		quests[def.id] = def;
	}

	bool AcceptQuest(const std::string& questID) {
		auto it = quests.find(questID);
		if (it == quests.end()) return false;

		QuestDef& quest = it->second;
		if (!ArePrerequisitesMet(quest)) return false;
		if (playerQuests[questID].status == QuestStatus::Active) return false;

		playerQuests[questID].status = QuestStatus::Active;
		if (onQuestAccepted) onQuestAccepted(quest);
		return true;
	}

	void UpdateObjective(const std::string& questID, const std::string& objectiveID, int amount = 1) {
		auto pq = playerQuests.find(questID);
		if (pq == playerQuests.end() || pq->second.status != QuestStatus::Active) return;

		for (auto& obj : quests[questID].objectives) {
			if (obj.id == objectiveID) {
				obj.Progress(amount);
				break;
			}
		}

		if (AreAllObjectivesComplete(questID)) {
			CompleteQuest(questID);
		}
	}

	void CompleteQuest(const std::string& questID) {
		playerQuests[questID].status = QuestStatus::Completed;
		if (onQuestCompleted) onQuestCompleted(quests[questID]);
	}

	void FailQuest(const std::string& questID) {
		playerQuests[questID].status = QuestStatus::Failed;
		if (onQuestFailed) onQuestFailed(quests[questID]);
	}

	QuestStatus GetQuestStatus(const std::string& questID) const {
		auto it = playerQuests.find(questID);
		return (it != playerQuests.end()) ? it->second.status : QuestStatus::Locked;
	}

	std::vector<QuestDef> GetActiveQuests() const {
		std::vector<QuestDef> result;
		for (auto& [id, pq] : playerQuests) {
			if (pq.status == QuestStatus::Active) result.push_back(quests.at(id));
		}
		return result;
	}

	std::function<void(const QuestDef&)> onQuestAccepted;
	std::function<void(const QuestDef&)> onQuestCompleted;
	std::function<void(const QuestDef&)> onQuestFailed;

private:
	std::unordered_map<std::string, QuestDef> quests;

	struct PlayerQuestState {
		QuestStatus status = QuestStatus::Locked;
	};
	std::unordered_map<std::string, PlayerQuestState> playerQuests;

	bool ArePrerequisitesMet(const QuestDef& quest) const {
		for (auto& preReq : quest.prerequisiteQuestIDs) {
			auto it = playerQuests.find(preReq);
			if (it == playerQuests.end() || it->second.status != QuestStatus::Completed) return false;
		}
		return true;
	}

	bool AreAllObjectivesComplete(const std::string& questID) const {
		auto it = quests.find(questID);
		if (it == quests.end()) return false;
		for (auto& obj : it->second.objectives) {
			if (!obj.isCompleted) return false;
		}
		return true;
	}
};

} // namespace arclight
