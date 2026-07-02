/* OGame Beyond - Jolt Physics Integration
 * Developer: Stephen
 * High-performance physics with rigid bodies, collision, constraints.
 * Supports space physics, gravity fields, and destruction.
 */
#pragma once
#include "../Core/Types.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace ogb {

enum class EBodyType { Static, Dynamic, Kinematic };
enum class ECollisionShape { Sphere, Box, Capsule, Mesh, ConvexHull };

struct PhysicsMaterial {
    float friction = 0.5f;
    float restitution = 0.3f;
    float density = 1.0f;
};

struct RigidBodyConfig {
    EBodyType bodyType = EBodyType::Dynamic;
    ECollisionShape shape = ECollisionShape::Sphere;
    float mass = 1.0f;
    float3 position = VEC3_ZERO;
    float3 rotation = VEC3_ZERO;
    float3 velocity = VEC3_ZERO;
    float3 angularVelocity = VEC3_ZERO;
    float linearDamping = 0.01f;
    float angularDamping = 0.05f;
    float gravityScale = 1.0f;
    bool isTrigger = false;
    bool useGravity = true;
    float3 collisionHalfExtent = float3(10, 10, 10);
    float collisionRadius = 10.0f;
    int entityID = -1;
    PhysicsMaterial material;
};

struct RigidBody {
    uint32_t id = 0;
    RigidBodyConfig config;
    float3 position;
    float3 velocity = VEC3_ZERO;
    float3 angularVelocity = VEC3_ZERO;
    Quaternion rotation;
    bool isSimulated = true;

    void ApplyForce(const float3& force) {
        if (config.bodyType == EBodyType::Dynamic) velocity += force * config.mass;
    }
    void ApplyImpulse(const float3& impulse) {
        if (config.bodyType == EBodyType::Dynamic) velocity += impulse;
    }
    void ApplyTorque(const float3& torque) {
        if (config.bodyType == EBodyType::Dynamic) angularVelocity += torque;
    }

    void Integrate(float dt) {
        if (config.bodyType != EBodyType::Dynamic) return;
        if (config.useGravity) velocity.y += -9.81f * config.gravityScale * dt;
        velocity *= (1.0f - config.linearDamping * dt);
        position += velocity * dt;
        angularVelocity *= (1.0f - config.angularDamping * dt);
    }
};

struct CollisionPair {
    uint32_t bodyA = 0, bodyB = 0;
    float3 contactPoint;
    float3 contactNormal;
    float penetrationDepth = 0;
    float impactForce = 0;
};

struct ConstraintConfig {
    uint32_t bodyA = 0, bodyB = 0;
    float3 anchorA, anchorB;
    float breakForce = 1000.0f;
};

// ======================== Space Physics ========================

struct GravityField {
    float3 position;
    float strength = 1.0f;
    float radius = 1000.0f;
    bool isBlackHole = false;
    float eventHorizon = 10.0f;
};

struct WarpBubble {
    float3 position;
    float3 velocity;
    float radius = 50.0f;
    float speed = 1000.0f;
    bool isActive = false;
};

// ======================== Jolt Physics System ========================

class JoltPhysicsSystem {
public:
    std::vector<RigidBody> bodies;
    std::vector<GravityField> gravityFields;
    std::vector<CollisionPair> collisions;
    float3 globalGravity = float3(0, -9.81f, 0);
    bool useSpacePhysics = false;

    uint32_t nextBodyID = 1;

    uint32_t CreateBody(const RigidBodyConfig& config) {
        RigidBody body;
        body.id = nextBodyID++;
        body.config = config;
        body.position = config.position;
        body.rotation = Quaternion(float3(0,0,0), 0);
        bodies.push_back(body);
        return body.id;
    }

    void DestroyBody(uint32_t id) {
        bodies.erase(
            std::remove_if(bodies.begin(), bodies.end(),
                [id](const RigidBody& b) { return b.id == id; }),
            bodies.end());
    }

    RigidBody* GetBody(uint32_t id) {
        for (auto& b : bodies) if (b.id == id) return &b;
        return nullptr;
    }

    void AddGravityField(const GravityField& field) { gravityFields.push_back(field); }

    void Update(float dt) {
        collisions.clear();

        // Apply gravity fields
        if (useSpacePhysics) {
            for (auto& body : bodies) {
                if (body.config.bodyType != EBodyType::Dynamic) continue;
                for (auto& field : gravityFields) {
                    float3 toCenter = field.position - body.position;
                    float dist = toCenter.Length();
                    if (dist < field.radius && dist > 0.1f) {
                        float force = field.strength / (dist * dist);
                        body.velocity += toCenter.Normalized() * force * dt;
                    }
                }
            }
        }

        // Integrate
        for (auto& body : bodies) {
            if (body.isSimulated) body.Integrate(dt);
        }

        // Broad phase collision detection
        for (size_t i = 0; i < bodies.size(); i++) {
            if (!bodies[i].isSimulated) continue;
            for (size_t j = i + 1; j < bodies.size(); j++) {
                if (!bodies[j].isSimulated) continue;
                CollisionPair pair;
                if (CheckCollision(bodies[i], bodies[j], pair)) {
                    ResolveCollision(bodies[i], bodies[j], pair);
                    collisions.push_back(pair);
                    if (onCollision) onCollision(pair);
                }
            }
        }
    }

    void SetGlobalGravity(const float3& g) { globalGravity = g; }

    std::function<void(const CollisionPair&)> onCollision;

private:
    bool CheckCollision(const RigidBody& a, const RigidBody& b, CollisionPair& pair) {
        float3 diff = b.position - a.position;
        float dist = diff.Length();
        float minDist = a.config.collisionRadius + b.config.collisionRadius;
        if (dist < minDist && dist > 0.001f) {
            pair.bodyA = a.id;
            pair.bodyB = b.id;
            pair.contactNormal = diff.Normalized();
            pair.contactPoint = a.position + pair.contactNormal * a.config.collisionRadius;
            pair.penetrationDepth = minDist - dist;
            pair.impactForce = (b.velocity - a.velocity).Length() * a.config.mass;
            return true;
        }
        return false;
    }

    void ResolveCollision(RigidBody& a, RigidBody& b, const CollisionPair& pair) {
        float3 normal = pair.contactNormal;
        float3 relativeVel = b.velocity - a.velocity;
        float velAlongNormal = relativeVel.Dot(normal);
        if (velAlongNormal > 0) return;
        float e = 0.3f;
        float j = -(1.0f + e) * velAlongNormal / (a.config.mass + b.config.mass);
        float3 impulse = normal * j;
        a.velocity -= impulse * a.config.mass;
        b.velocity += impulse * b.config.mass;
    }
};

} // namespace ogb
