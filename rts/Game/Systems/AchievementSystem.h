/* ArcLight Engine - Achievement System
 * Developer: Stephen
 * Tracks player accomplishments, milestones, and rewards.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace arclight {

enum class AchievementRarity {
	Bronze,
	Silver,
	Gold,
	Platinum,
	Diamond,
};

struct AchievementDef {
	std::string id;
	std::string name;
	std::string description;
	AchievementRarity rarity = AchievementRarity::Bronze;
	bool isHidden = false;
	int rewardXP = 0;
	int rewardCredits = 0;
};

struct PlayerAchievement {
	std::string achievementID;
	bool unlocked = false;
	float progress = 0.0f; // 0.0 - 1.0
	std::string unlockedAt;
};

class AchievementSystem {
public:
	void RegisterAchievement(const AchievementDef& def) {
		achievements[def.id] = def;
	}

	void SetProgress(const std::string& id, float progress) {
		auto it = playerAchievements.find(id);
		if (it == playerAchievements.end()) {
			PlayerAchievement pa;
			pa.achievementID = id;
			pa.progress = progress;
			if (progress >= 1.0f) {
				pa.unlocked = true;
				pa.unlockedAt = GetCurrentTime();
				UnlockReward(achievements[id]);
			}
			playerAchievements[id] = pa;
		} else if (!it->second.unlocked) {
			it->second.progress = progress;
			if (progress >= 1.0f) {
				it->second.unlocked = true;
				it->second.unlockedAt = GetCurrentTime();
				UnlockReward(achievements[id]);
			}
		}
	}

	void IncrementProgress(const std::string& id, float amount) {
		auto it = playerAchievements.find(id);
		float current = (it != playerAchievements.end()) ? it->second.progress : 0.0f;
		SetProgress(id, current + amount);
	}

	bool IsUnlocked(const std::string& id) const {
		auto it = playerAchievements.find(id);
		return it != playerAchievements.end() && it->second.unlocked;
	}

	float GetProgress(const std::string& id) const {
		auto it = playerAchievements.find(id);
		return (it != playerAchievements.end()) ? it->second.progress : 0.0f;
	}

	int GetUnlockedCount() const {
		int count = 0;
		for (auto& [id, pa] : playerAchievements) {
			if (pa.unlocked) count++;
		}
		return count;
	}

	int GetTotalCount() const { return static_cast<int>(achievements.size()); }

	std::vector<AchievementDef> GetAllAchievements() const {
		std::vector<AchievementDef> result;
		for (auto& [id, def] : achievements) result.push_back(def);
		return result;
	}

	std::function<void(const AchievementDef&)> onAchievementUnlocked;

private:
	std::unordered_map<std::string, AchievementDef> achievements;
	std::unordered_map<std::string, PlayerAchievement> playerAchievements;

	void UnlockReward(const AchievementDef& def) {
		if (onAchievementUnlocked) onAchievementUnlocked(def);
	}

	std::string GetCurrentTime() { return ""; }
};

} // namespace arclight
