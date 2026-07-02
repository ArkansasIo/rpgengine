/* OGame Beyond - Battle System
 * Developer: Stephen
 * Real-time combat, fleet battles, and damage resolution.
 */
#pragma once
#include "../../Engine/Core/Types.h"
#include <string>
#include <vector>

namespace ogb {

enum class EWeaponType { Laser, Missile, Torpedo, Beam, Flak, Railgun, NovaCannon, IonCannon };
enum class ECombatState { Idle, Engaging, Pursuing, Fleeing, FormingUp };

struct WeaponSystem {
    std::string id;
    EWeaponType type = EWeaponType::Laser;
    float damage = 10;
    float range = 500;
    float fireRate = 1.0f;
    float fireCooldown = 0;
    float accuracy = 0.9f;
    float projectileSpeed = 1000;
    float aoeRadius = 0;
    float armorPenetration = 0;
    bool isTracking = false;
    float trackingStrength = 0;

    bool CanFire() const { return fireCooldown <= 0; }
    void UpdateCooldown(float dt) { fireCooldown = std::max(0.0f, fireCooldown - dt); }
    void Fire() { fireCooldown = 1.0f / fireRate; }
};

struct Projectile {
    Vec3 position;
    Vec3 velocity;
    float damage = 0;
    float lifetime = 3.0f;
    float age = 0;
    EWeaponType type = EWeaponType::Laser;
    int ownerFleetID = -1;
    bool isActive = true;

    void Update(float dt) {
        age += dt;
        if (age >= lifetime) { isActive = false; return; }
        position += velocity * dt;
    }
};

struct BattleResult {
    int winnerFleetID = -1;
    int loserFleetID = -1;
    float winnerLosses = 0;
    float loserLosses = 0;
    float duration = 0;
    float xpGained = 0;
};

class BattleSystem {
public:
    std::vector<Projectile> projectiles;
    std::vector<BattleResult> battleLog;

    void FireWeapon(const Vec3& origin, const Vec3& direction, float damage, EWeaponType type, float speed, int fleetID) {
        Projectile p;
        p.position = origin;
        p.velocity = direction.Normalized() * speed;
        p.damage = damage;
        p.type = type;
        p.ownerFleetID = fleetID;
        projectiles.push_back(p);
    }

    void Update(float dt) {
        for (auto& p : projectiles) p.Update(dt);
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                [](const Projectile& p) { return !p.isActive; }),
            projectiles.end());
    }

    bool CheckHit(const Vec3& targetPos, float targetRadius, int targetFleetID) {
        for (auto& p : projectiles) {
            if (!p.isActive || p.ownerFleetID == targetFleetID) continue;
            if ((p.position - targetPos).Length() < targetRadius) {
                p.isActive = false;
                return true;
            }
        }
        return false;
    }

    float CalculateDamage(float baseDamage, float armor, float shield, float armorPen) {
        float shieldAbsorb = std::min(shield, baseDamage);
        float remaining = baseDamage - shieldAbsorb;
        float armorReduction = armor * (1.0f - std::min(1.0f, armorPen));
        return std::max(0.0f, remaining - armorReduction);
    }
};

} // namespace ogb
