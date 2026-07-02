/* OGame Beyond - AActor
 * Developer: Stephen
 * Base class for all placeable game objects with components and tick.
 */
#pragma once
#include "UObject.h"
#include "../ECS/Entity.h"
#include <vector>
#include <memory>

namespace ogb {
class UActorComponent;

class AActor : public UObject {
public:
    AActor() { SetName("Actor"); }
    virtual ~AActor() = default;

    const char* GetClassName() const override { return "AActor"; }

    virtual void BeginPlay() {}
    virtual void Tick(float dt) {
        for (auto& comp : components) comp->Tick(dt);
    }
    virtual void EndPlay() {}

    // Component management
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->owner = this;
        T* ptr = comp.get();
        components.push_back(std::move(comp));
        return ptr;
    }

    template<typename T>
    T* FindComponent() const {
        for (auto& c : components) {
            T* cast = dynamic_cast<T*>(c.get());
            if (cast) return cast;
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<UActorComponent>>& GetComponents() const { return components; }

    // Transform
    Vec3 GetActorLocation() const { return location; }
    void SetActorLocation(const Vec3& loc) { location = loc; }
    Vec3 GetActorRotation() const { return rotation; }
    void SetActorRotation(const Vec3& rot) { rotation = rot; }
    float GetActorScale() const { return scale; }
    void SetActorScale(float s) { scale = s; }

    // State
    bool IsAlive() const { return !pendingKill; }
    bool IsActive() const { return active; }
    void SetActive(bool a) { active = a; }

    // Collision
    bool HasCollision() const { return hasCollision; }
    void SetCollision(bool c) { hasCollision = c; }
    float GetCollisionRadius() const { return collisionRadius; }

    // Team
    int GetTeamID() const { return teamID; }
    void SetTeamID(int t) { teamID = t; }

protected:
    Vec3 location;
    Vec3 rotation;
    float scale = 1.0f;
    bool active = true;
    bool hasCollision = true;
    float collisionRadius = 10.0f;
    int teamID = -1;
    std::vector<std::unique_ptr<UActorComponent>> components;
};
} // namespace ogb
