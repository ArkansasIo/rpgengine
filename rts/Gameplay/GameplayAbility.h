/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Gameplay Ability System (GAS).
 * Abilities are modular actions with costs, cooldowns, and effects.
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace arclight {

using AbilityID = uint32_t;

enum class AbilityActivationPolicy {
	/** Activated by player/AI input */
	OnInput,
	/** Activated automatically when conditions are met */
	AutoActivate,
	/** Passive ability, always active while equipped */
	Passive,
	/** Triggered by an event (damage, kill, etc.) */
	OnEvent,
};

enum class AbilityCostType {
	Mana,
	Energy,
	Health,
	Cooldown,
	Custom,
};

struct AbilityCost {
	AbilityCostType type = AbilityCostType::Mana;
	float amount = 0.0f;
	std::string resourceName;
};

struct AbilityCooldown {
	float duration = 0.0f;
	float remaining = 0.0f;
	bool IsReady() const { return remaining <= 0.0f; }
	void Tick(float dt) { if (remaining > 0.0f) remaining -= dt; }
	void Trigger() { remaining = duration; }
};

struct GameplayTag {
	std::string tag; // hierarchical: "Ability.Attack.Melee"
	bool operator==(const GameplayTag& o) const { return tag == o.tag; }
};

class GameplayAbility {
public:
	std::string abilityName;
	AbilityID id = 0;
	AbilityActivationPolicy activationPolicy = AbilityActivationPolicy::OnInput;
	bool isActive = false;
	bool canBeInterrupted = true;
	bool blockOtherAbilities = false;
	float duration = 0.0f; // -1 = indefinite
	float elapsed = 0.0f;
	int priority = 0;

	std::vector<AbilityCost> costs;
	AbilityCooldown cooldown;
	std::vector<GameplayTag> tags;
	std::vector<GameplayTag> cancelTags; // tags that cancel this ability

	// Callbacks
	std::function<bool()> canActivate;
	std::function<void()> onActivate;
	std::function<void()> onDeactivate;
	std::function<void(float)> onTick; // per-frame while active
	std::function<void()> onCancel;

	bool CanActivate() const {
		if (isActive) return false;
		if (!cooldown.IsReady()) return false;
		if (canActivate) return canActivate();
		return true;
	}

	void Activate() {
		if (!CanActivate()) return;
		isActive = true;
		elapsed = 0.0f;
		cooldown.Trigger();
		if (onActivate) onActivate();
	}

	void Deactivate() {
		if (!isActive) return;
		isActive = false;
		if (onDeactivate) onDeactivate();
	}

	void Tick(float dt) {
		if (!isActive) return;
		elapsed += dt;
		cooldown.Tick(dt);
		if (onTick) onTick(dt);
		if (duration > 0.0f && elapsed >= duration) {
			Deactivate();
		}
	}

	void Cancel() {
		if (!isActive) return;
		if (onCancel) onCancel();
		isActive = false;
	}

	void UpdateCooldown(float dt) {
		cooldown.Tick(dt);
	}

	bool HasTag(const GameplayTag& tag) const {
		for (auto& t : tags) {
			if (t == tag) return true;
		}
		return false;
	}
};

} // namespace arclight
