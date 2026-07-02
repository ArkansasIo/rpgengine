/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Health Component with damage types, shields, armor,
 * and event callbacks for gameplay reactions.
 */

#pragma once

#include "../Entity.h"
#include <functional>
#include <vector>
#include <string>

namespace arclight {

enum class DamageType {
	Normal,
	Explosive,
	ArmorPiercing,
	Energy,
	EMP,
	Heal,
	TrueDamage // ignores all mitigation
};

struct DamageEvent {
	EntityID attacker = INVALID_ENTITY;
	EntityID victim = INVALID_ENTITY;
	float baseDamage = 0.0f;
	DamageType type = DamageType::Normal;
	float3 hitDirection = ZeroVector;
	float criticalMultiplier = 1.0f;
};

using DamageCallback = std::function<void(const DamageEvent&)>;

struct HealthComponent : public IComponent {
	float maxHealth = 100.0f;
	float currentHealth = 100.0f;
	float maxShield = 0.0f;
	float currentShield = 0.0f;
	float shieldRechargeRate = 0.0f;
	float shieldRechargeDelay = 2.0f; // seconds after last hit
	float armor = 0.0f; // flat damage reduction
	float armorRating = 0.0f; // percentage reduction (0-1)
	bool invulnerable = false;
	bool isDead = false;

	// Damage type resistances (multiplier, 0 = immune, 0.5 = half, 2.0 = double)
	float damageMultipliers[6] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

	float timeSinceLastHit = 999.0f;

	std::vector<DamageCallback> onDamageCallbacks;
	std::vector<DamageCallback> onDeathCallbacks;
	std::vector<std::function<void(float)>> onHealCallbacks;

	void UpdateShieldRecharge(float dt) {
		if (maxShield <= 0.0f) return;
		timeSinceLastHit += dt;
		if (timeSinceLastHit >= shieldRechargeDelay && currentShield < maxShield) {
			currentShield = std::min(maxShield, currentShield + shieldRechargeRate * dt);
		}
	}

	float ApplyDamage(const DamageEvent& event) {
		if (invulnerable || isDead) return 0.0f;

		float damage = event.baseDamage * event.criticalMultiplier;

		// Apply damage type multiplier
		int typeIdx = static_cast<int>(event.type);
		if (typeIdx >= 0 && typeIdx < 6) {
			damage *= damageMultipliers[typeIdx];
		}

		// True damage bypasses armor
		if (event.type != DamageType::TrueDamage) {
			// Shield absorption first
			if (currentShield > 0.0f) {
				float shieldAbsorb = std::min(currentShield, damage);
				currentShield -= shieldAbsorb;
				damage -= shieldAbsorb;
			}

			// Flat armor reduction
			damage = std::max(0.0f, damage - armor);

			// Percentage armor rating
			damage *= (1.0f - armorRating);
		}

		timeSinceLastHit = 0.0f;
		currentHealth -= damage;

		for (auto& cb : onDamageCallbacks) {
			cb(event);
		}

		if (currentHealth <= 0.0f) {
			currentHealth = 0.0f;
			isDead = true;
			for (auto& cb : onDeathCallbacks) {
				cb(event);
			}
		}

		return damage;
	}

	void Heal(float amount, EntityID /*healer*/ = INVALID_ENTITY) {
		if (isDead || amount <= 0.0f) return;
		float actual = std::min(amount, maxHealth - currentHealth);
		currentHealth += actual;
		for (auto& cb : onHealCallbacks) {
			cb(actual);
		}
	}

	float GetHealthPercent() const {
		return maxHealth > 0.0f ? currentHealth / maxHealth : 0.0f;
	}

	float GetShieldPercent() const {
		return maxShield > 0.0f ? currentShield / maxShield : 0.0f;
	}

	bool IsAlive() const { return !isDead && currentHealth > 0.0f; }
};

} // namespace arclight
