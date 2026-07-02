/* OGame Beyond - UActorComponent
 * Developer: Stephen
 * Base class for all components attached to actors.
 */
#pragma once
#include "UObject.h"

namespace ogb {
class AActor;

class UActorComponent : public UObject {
public:
    virtual ~UActorComponent() = default;
    const char* GetClassName() const override { return "UActorComponent"; }

    virtual void BeginPlay() {}
    virtual void Tick(float dt) { (void)dt; }
    virtual void EndPlay() {}

    AActor* GetOwner() const { return owner; }
    void SetOwner(AActor* a) { owner = a; }
    bool IsActive() const { return active; }
    void SetActive(bool a) { active = a; }

protected:
    AActor* owner = nullptr;
    bool active = true;
};
} // namespace ogb
