/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Weapon Component - modular weapon system
 * supporting firing modes, cooldowns, ammo, targeting, and projectiles.
 */

#pragma once

#include "../Entity.h"
#include "../../System/float3.h"
#include "HealthComponent.h"
#include <vector>
#include <string>
#include <functional>

namespace arclight {

enum class FireMode {
	Single,       // one shot per click
	Burst,        // N shots per click
	Auto,         // continuous fire while held
	Charge,       // hold to charge, release to fire
	Beam,         // continuous beam
	Missile,      // lock-on guided
};

enum class TargetingMode {
	None,
	Closest,
	Strongest,
	Weakest,
	NearestEnemy,
	Player指定,
	Ground,       // ground-targeted
	Air,          // air-only
};

struct ProjectileDef {
	float speed = 300.0f;
	float gravity = 0.0f;
	float lifetime = 3.0f;
	float turnRate = 0.0f; // for guided missiles
	float aoeRadius = 0.0f;
	float aoeFalloff = 1.0f;
	int penetration = 0; // how many targets it can pass through
	bool homing = false;
	float homingStrength = 1.0f;
	std::string impactEffect;
	std::string trailEffect;
};

struct WeaponComponent : public IComponent {
	std::string weaponName = "default";
	float damage = 10.0f;
	DamageType damageType = DamageType::Normal;
	float range = 500.0f;
	float spread = 0.0f; // degrees of inaccuracy
	float projectilesPerShot = 1;
	float fireRate = 1.0f; // shots per second
	float fireCooldown = 0.0f;
	FireMode fireMode = FireMode::Single;
	TargetingMode targetingMode = TargetingMode::Closest;

	// Burst mode
	int burstCount = 3;
	int burstShotsFired = 0;
	float burstDelay = 0.1f;

	// Charge mode
	float chargeTime = 1.0f;
	float currentCharge = 0.0f;
	float chargeDamageMultiplier = 3.0f;

	// Ammo
	int currentAmmo = -1; // -1 = infinite
	int maxAmmo = -1;
	float reloadTime = 0.0f;
	float reloadCooldown = 0.0f;
	bool infiniteAmmo = true;

	// Projectile
	ProjectileDef projectileDef;

	// Beam weapons
	float beamWidth = 2.0f;
	float beamDps = 20.0f;
	bool beamHitscan = true;

	// Critical hits
	float criticalChance = 0.05f;
	float criticalMultiplier = 2.0f;

	// Weapon slot
	int slotIndex = 0;
	bool isActive = true;
	bool isReloading = false;

	// Projectile spawn offset from unit center
	float3 muzzleOffset = ZeroVector;

	// Events
	std::function<void(EntityID, const float3&)> onFire;
	std::function<void(EntityID, EntityID)> onHit;

	void UpdateCooldown(float dt) {
		if (fireCooldown > 0.0f) {
			fireCooldown -= dt;
		}
		if (isReloading) {
			reloadCooldown -= dt;
			if (reloadCooldown <= 0.0f) {
				isReloading = false;
				currentAmmo = maxAmmo;
			}
		}
	}

	bool CanFire() const {
		if (!isActive) return false;
		if (fireCooldown > 0.0f) return false;
		if (isReloading) return false;
		if (!infiniteAmmo && currentAmmo <= 0) return false;
		return true;
	}

	void ConsumeAmmo() {
		if (!infiniteAmmo && currentAmmo > 0) {
			currentAmmo--;
			if (currentAmmo <= 0 && reloadTime > 0.0f) {
				isReloading = true;
				reloadCooldown = reloadTime;
			}
		}
	}

	float GetDamage() const {
		float dmg = damage;
		if (fireMode == FireMode::Charge) {
			float chargeRatio = chargeTime > 0.0f ? currentCharge / chargeTime : 1.0f;
			dmg *= 1.0f + (chargeDamageMultiplier - 1.0f) * chargeRatio;
		}
		return dmg;
	}

	void InitiateFire() {
		fireCooldown = 1.0f / fireRate;
		ConsumeAmmo();
	}
};

} // namespace arclight
