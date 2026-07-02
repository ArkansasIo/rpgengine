/* OGame Beyond - APawn
 * Developer: Stephen
 * Base class for controllable entities (ships, stations, etc).
 */
#pragma once
#include "AActor.h"

namespace ogb {
class AController;

class APawn : public AActor {
public:
    APawn() { SetName("Pawn"); }
    const char* GetClassName() const override { return "APawn"; }

    virtual void PossessedBy(AController* ctrl) { controller = ctrl; }
    virtual void UnPossessed() { controller = nullptr; }

    AController* GetController() const { return controller; }

    void AddMovementInput(const Vec3& dir, float scale = 1.0f) {
        pendingMovement += dir * scale;
    }

    Vec3 GetPendingMovement() const { return pendingMovement; }
    void ConsumeMovement() { pendingMovement = VEC3_ZERO; }

    bool IsPossessed() const { return controller != nullptr; }

protected:
    AController* controller = nullptr;
    Vec3 pendingMovement;
};
} // namespace ogb
