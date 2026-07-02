/* OGame Beyond - AController
 * Developer: Stephen
 * Base controller class for AI and player control.
 */
#pragma once
#include "AActor.h"

namespace ogb {
class APawn;

class AController : public AActor {
public:
    AController() { SetName("Controller"); }
    const char* GetClassName() const override { return "AController"; }

    virtual void Possess(APawn* pawn) {
        if (possessedPawn) UnPossess();
        possessedPawn = pawn;
        if (pawn) pawn->PossessedBy(this);
    }

    virtual void UnPossess() {
        if (possessedPawn) {
            possessedPawn->UnPossessed();
            possessedPawn = nullptr;
        }
    }

    APawn* GetPawn() const { return possessedPawn; }
    bool HasPawn() const { return possessedPawn != nullptr; }

protected:
    APawn* possessedPawn = nullptr;
};
} // namespace ogb
