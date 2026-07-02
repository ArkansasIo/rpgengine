/* OGame Beyond - ACharacter
 * Developer: Stephen
 * Character class with movement, health, and animation support.
 */
#pragma once
#include "APawn.h"

namespace ogb {
class UCharacterMovementComponent;

struct CharacterStats {
    float health = 100;
    float maxHealth = 100;
    float shield = 0;
    float maxShield = 0;
    float armor = 0;
    float speed = 10;
    float jumpHeight = 5;
    float sightRange = 50;
};

class ACharacter : public APawn {
public:
    ACharacter() { SetName("Character"); }
    const char* GetClassName() const override { return "ACharacter"; }

    void BeginPlay() override {
        stats.health = stats.maxHealth;
        stats.shield = stats.maxShield;
    }

    CharacterStats& GetStats() { return stats; }
    const CharacterStats& GetStats() const { return stats; }

    void Jump() {
        if (isGrounded) {
            velocity.y = stats.jumpHeight;
            isGrounded = false;
        }
    }

    void TakeDamage(float amount, float armorPen = 0) {
        float mitigated = amount * (1.0f - std::min(1.0f, stats.armor * (1.0f - armorPen)));
        float shieldAbsorb = std::min(stats.shield, mitigated);
        stats.shield -= shieldAbsorb;
        stats.health -= (mitigated - shieldAbsorb);
        if (stats.health <= 0) { stats.health = 0; OnDeath(); }
    }

    void Heal(float amount) {
        stats.health = std::min(stats.maxHealth, stats.health + amount);
    }

    bool IsDead() const { return stats.health <= 0; }
    float GetHealthPercent() const { return stats.maxHealth > 0 ? stats.health / stats.maxHealth : 0; }

protected:
    CharacterStats stats;
    Vec3 velocity;
    bool isGrounded = true;

    virtual void OnDeath() {}
};
} // namespace ogb
