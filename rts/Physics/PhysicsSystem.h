/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Physics System with rigid body dynamics,
 * collision detection, constraints, and vehicle physics.
 */

#pragma once

#include "../System/float3.h"
#include <vector>
#include <functional>

namespace arclight {

enum class ECollisionShape {
	Sphere,
	Box,
	Capsule,
	Mesh,
};

enum class EBodyType {
	Static,      // immovable (buildings, terrain)
	Dynamic,     // fully simulated (debris, projectiles)
	Kinematic,   // moved by code, affects others (doors, platforms)
};

struct RigidBody {
	float3 position = ZeroVector;
	float3 velocity = ZeroVector;
	float3 angularVelocity = ZeroVector;
	Quaternion rotation;
	float mass = 1.0f;
	float inverseMass = 1.0f; // precomputed for efficiency
	float3 inertiaTensor = float3(1.0f);
	float3 inverseInertiaTensor = float3(1.0f);
	float linearDamping = 0.01f;
	float angularDamping = 0.05f;
	float gravityScale = 1.0f;
	EBodyType bodyType = EBodyType::Dynamic;
	ECollisionShape collisionShape = ECollisionShape::Sphere;
	float collisionRadius = 10.0f;
	float3 collisionBoxHalfExtent = float3(10.0f);
	int entityID = -1;

	bool isSimulated = true;
	bool isTrigger = false; // overlap only, no physics response
	bool useGravity = true;

	void ApplyForce(const float3& force) {
		if (bodyType != EBodyType::Dynamic) return;
		velocity += force * inverseMass;
	}

	void ApplyImpulse(const float3& impulse) {
		if (bodyType != EBodyType::Dynamic) return;
		velocity += impulse * inverseMass;
	}

	void ApplyTorque(const float3& torque) {
		if (bodyType != EBodyType::Dynamic) return;
		angularVelocity += float3(
			torque.x * inverseInertiaTensor.x,
			torque.y * inverseInertiaTensor.y,
			torque.z * inverseInertiaTensor.z
		);
	}

	void ApplyAngularImpulse(const float3& impulse) {
		if (bodyType != EBodyType::Dynamic) return;
		angularVelocity += float3(
			impulse.x * inverseInertiaTensor.x,
			impulse.y * inverseInertiaTensor.y,
			impulse.z * inverseInertiaTensor.z
		);
	}

	void Integrate(float dt) {
		if (bodyType != EBodyType::Dynamic) return;

		// Linear
		if (useGravity) {
			velocity.y += -9.81f * gravityScale * dt;
		}
		velocity *= (1.0f - linearDamping * dt);
		position += velocity * dt;

		// Angular
		angularVelocity *= (1.0f - angularDamping * dt);
		// Simplified rotation integration
		float angle = angularVelocity.Length() * dt;
		if (angle > 0.0001f) {
			float3 axis = angularVelocity.Normalize();
			rotation = rotation * Quaternion(axis, angle);
			rotation = rotation.Normalized();
		}
	}
};

struct CollisionInfo {
	int bodyA = -1;
	int bodyB = -1;
	float3 contactPoint = ZeroVector;
	float3 contactNormal = ZeroVector;
	float penetrationDepth = 0.0f;
};

using CollisionCallback = std::function<void(const CollisionInfo&)>;

struct Constraint {
	int bodyA = -1;
	int bodyB = -1;
	float3 anchorA = ZeroVector;
	float3 anchorB = ZeroVector;
	float breakForce = 1000.0f;
	bool isBroken = false;
};

class PhysicsSystem {
public:
	static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
	static constexpr int MAX_SUBSTEPS = 4;

	void SetGravity(const float3& g) { gravity = g; }

	int AddBody(const RigidBody& body) {
		bodies.push_back(body);
		return static_cast<int>(bodies.size()) - 1;
	}

	void RemoveBody(int index) {
		if (index >= 0 && index < static_cast<int>(bodies.size())) {
			bodies[index].isSimulated = false;
		}
	}

	RigidBody* GetBody(int index) {
		if (index >= 0 && index < static_cast<int>(bodies.size())) {
			return &bodies[index];
		}
		return nullptr;
	}

	void AddConstraint(const Constraint& constraint) {
		constraints.push_back(constraint);
	}

	void SetCollisionCallback(CollisionCallback cb) {
		onCollision = cb;
	}

	void Update(float dt) {
		accumulator += dt;
		int steps = 0;
		while (accumulator >= FIXED_TIMESTEP && steps < MAX_SUBSTEPS) {
			StepPhysics(FIXED_TIMESTEP);
			accumulator -= FIXED_TIMESTEP;
			steps++;
		}
	}

private:
	float3 gravity = float3(0.0f, -9.81f, 0.0f);
	std::vector<RigidBody> bodies;
	std::vector<Constraint> constraints;
	CollisionCallback onCollision;
	float accumulator = 0.0f;

	void StepPhysics(float dt) {
		// Integrate
		for (auto& body : bodies) {
			if (body.isSimulated) {
				body.Integrate(dt);
			}
		}

		// Broad phase collision (simple all-pairs, use spatial hash for production)
		for (int i = 0; i < static_cast<int>(bodies.size()); i++) {
			if (!bodies[i].isSimulated) continue;
			for (int j = i + 1; j < static_cast<int>(bodies.size()); j++) {
				if (!bodies[j].isSimulated) continue;

				CollisionInfo info;
				if (CheckCollision(bodies[i], bodies[j], info)) {
					if (!bodies[i].isTrigger && !bodies[j].isTrigger) {
						ResolveCollision(bodies[i], bodies[j], info);
					}
					if (onCollision) {
						onCollision(info);
					}
				}
			}
		}

		// Solve constraints
		for (auto& constraint : constraints) {
			if (constraint.isBroken) continue;
			SolveConstraint(constraint);
		}
	}

	bool CheckCollision(const RigidBody& a, const RigidBody& b, CollisionInfo& info) {
		float3 diff = b.position - a.position;
		float dist = diff.Length();
		float minDist = a.collisionRadius + b.collisionRadius;

		if (dist < minDist && dist > 0.001f) {
			info.bodyA = a.entityID;
			info.bodyB = b.entityID;
			info.contactNormal = diff.Normalize();
			info.contactPoint = a.position + info.contactNormal * a.collisionRadius;
			info.penetrationDepth = minDist - dist;
			return true;
		}
		return false;
	}

	void ResolveCollision(RigidBody& a, RigidBody& b, const CollisionInfo& info) {
		float3 normal = info.contactNormal;
		float3 relativeVel = b.velocity - a.velocity;
		float velAlongNormal = relativeVel.Dot(normal);

		// Don't resolve if moving apart
		if (velAlongNormal > 0) return;

		float e = 0.3f; // restitution
		float j = -(1.0f + e) * velAlongNormal;
		j /= a.inverseMass + b.inverseMass;

		float3 impulse = normal * j;
		a.velocity -= impulse * a.inverseMass;
		b.velocity += impulse * b.inverseMass;

		// Positional correction (prevent sinking)
		const float percent = 0.8f;
		const float slop = 0.01f;
		float3 correction = normal * (std::max(info.penetrationDepth - slop, 0.0f) / (a.inverseMass + b.inverseMass) * percent);
		a.position -= correction * a.inverseMass;
		b.position += correction * b.inverseMass;
	}

	void SolveConstraint(Constraint& constraint) {
		auto* bodyA = GetBody(constraint.bodyA);
		auto* bodyB = GetBody(constraint.bodyB);
		if (!bodyA || !bodyB) return;

		float3 worldAnchorA = bodyA->position + constraint.anchorA;
		float3 worldAnchorB = bodyB->position + constraint.anchorB;
		float3 delta = worldAnchorB - worldAnchorA;
		float distance = delta.Length();

		if (distance > constraint.breakForce * 0.01f) {
			constraint.isBroken = true;
			return;
		}

		if (distance > 0.001f) {
			float3 correction = delta * (distance * 0.5f);
			bodyA->position += correction * bodyA->inverseMass;
			bodyB->position -= correction * bodyB->inverseMass;
		}
	}
};

} // namespace arclight
