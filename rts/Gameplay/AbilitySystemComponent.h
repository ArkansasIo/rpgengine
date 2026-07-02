/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Ability System Component.
 * Manages a collection of GameplayAbilities for an entity.
 * Handles activation, costs, cooldowns, and tag queries.
 */

#pragma once

#include "GameplayAbility.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace arclight {

class AbilitySystemComponent {
public:
	void AddAbility(std::unique_ptr<GameplayAbility> ability) {
		ability->id = nextAbilityID++;
		abilities.push_back(std::move(ability));
	}

	bool TryActivateAbility(AbilityID id) {
		for (auto& ability : abilities) {
			if (ability->id == id) {
				if (ability->CanActivate()) {
					// Check block tags
					for (auto& tag : activeTags) {
						for (auto& cancelTag : ability->cancelTags) {
							if (tag == cancelTag) return false;
						}
					}

					// Deactivate blocking abilities
					if (ability->blockOtherAbilities) {
						for (auto& other : abilities) {
							if (other->isActive && other->canBeInterrupted) {
								other->Cancel();
							}
						}
					}

					ability->Activate();
					if (ability->activationPolicy != AbilityActivationPolicy::Passive) {
						activeTags.push_back({ability->abilityName});
					}
					return true;
				}
			}
		}
		return false;
	}

	void DeactivateAbility(AbilityID id) {
		for (auto& ability : abilities) {
			if (ability->id == id && ability->isActive) {
				ability->Deactivate();
				// Remove from active tags
				activeTags.erase(
					std::remove_if(activeTags.begin(), activeTags.end(),
						[&](const GameplayTag& t) { return t.tag == ability->abilityName; }
					),
					activeTags.end()
				);
				return;
			}
		}
	}

	void CancelAbilityByTag(const GameplayTag& tag) {
		for (auto& ability : abilities) {
			if (ability->isActive && ability->HasTag(tag)) {
				ability->Cancel();
			}
		}
	}

	void Update(float dt) {
		for (auto& ability : abilities) {
			if (ability->isActive) {
				ability->Tick(dt);
			}
			ability->UpdateCooldown(dt);
		}
	}

	bool HasActiveAbility() const {
		for (auto& ability : abilities) {
			if (ability->isActive) return true;
		}
		return false;
	}

	bool HasTag(const GameplayTag& tag) const {
		for (auto& t : activeTags) {
			if (t == tag) return true;
		}
		return false;
	}

	std::vector<GameplayAbility*> GetActiveAbilities() {
		std::vector<GameplayAbility*> result;
		for (auto& ability : abilities) {
			if (ability->isActive) result.push_back(ability.get());
		}
		return result;
	}

	GameplayAbility* FindAbilityByName(const std::string& name) {
		for (auto& ability : abilities) {
			if (ability->abilityName == name) return ability.get();
		}
		return nullptr;
	}

private:
	std::vector<std::unique_ptr<GameplayAbility>> abilities;
	std::vector<GameplayTag> activeTags;
	AbilityID nextAbilityID = 1;
};

} // namespace arclight
