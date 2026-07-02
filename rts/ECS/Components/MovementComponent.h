/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Movement Component with acceleration, braking,
 * pathfinding integration, and movement modes.
 */

#pragma once

#include "../Entity.h"
#include "Types.h"

namespace arclight {

enum class MovementMode {
	Walking,
	Flying,
	Swimming,
	Floating,    // zero-g / space
	OnRails,     // constrained to path
	Stationary,  // immovable
};

struct MovementComponent : public IComponent {
	MovementMode movementMode = MovementMode::Walking;
	float maxSpeed = 100.0f;
	float acceleration = 200.0f;
	float brakingDeceleration = 300.0f;
	float turnRate = 180.0f; // degrees per second

	float3 velocity = ZeroVector;
	float3 targetPosition = ZeroVector;
	float3 moveDirection = ZeroVector;

	bool isMoving = false;
	bool hasPathTarget = false;
	float arrivalDistance = 10.0f;
	float stoppingDistance = 5.0f;

	// Path following
	std::vector<float3> pathWaypoints;
	int currentWaypointIndex = 0;

	// Rotation control
	bool rotateToFaceMovement = true;
	bool useSmoothRotation = true;
	float rotationSmoothing = 5.0f;

	void SetTarget(const float3& target) {
		targetPosition = target;
		hasPathTarget = true;
	}

	void ClearTarget() {
		hasPathTarget = false;
		velocity = ZeroVector;
		isMoving = false;
		pathWaypoints.clear();
		currentWaypointIndex = 0;
	}

	void SetPath(const std::vector<float3>& waypoints) {
		pathWaypoints = waypoints;
		currentWaypointIndex = 0;
		hasPathTarget = !waypoints.empty();
	}

	float3 UpdateMovement(float dt, const float3& currentPos) {
		if (movementMode == MovementMode::Stationary) {
			return ZeroVector;
		}

		// Follow waypoints
		if (hasPathTarget && !pathWaypoints.empty()) {
			if (currentWaypointIndex < static_cast<int>(pathWaypoints.size())) {
				targetPosition = pathWaypoints[currentWaypointIndex];
				float dist = (targetPosition - currentPos).Length();
				if (dist < arrivalDistance) {
					currentWaypointIndex++;
					if (currentWaypointIndex >= static_cast<int>(pathWaypoints.size())) {
						ClearTarget();
						return ZeroVector;
					}
				}
			}
		}

		// Calculate desired direction
		float3 toTarget = targetPosition - currentPos;
		float distance = toTarget.Length();

		if (distance < stoppingDistance) {
			// Apply braking
			if (velocity.Length() > 0.1f) {
				float3 brakeForce = -velocity.Normalize() * brakingDeceleration * dt;
				if (brakeForce.Length() > velocity.Length()) {
					velocity = ZeroVector;
				} else {
					velocity += brakeForce;
				}
			}
			isMoving = false;
			return velocity;
		}

		moveDirection = toTarget.Normalize();
		isMoving = true;

		// Accelerate towards target
		float currentSpeed = velocity.Length();
		float desiredSpeed = std::min(maxSpeed, distance); // slow down near target

		if (currentSpeed < desiredSpeed) {
			velocity += moveDirection * acceleration * dt;
			if (velocity.Length() > desiredSpeed) {
				velocity = velocity.Normalize() * desiredSpeed;
			}
		} else if (currentSpeed > desiredSpeed) {
			velocity = velocity.Normalize() * desiredSpeed;
		}

		return velocity;
	}

	float3 GetCurrentSpeed() const { return velocity; }
	float GetCurrentSpeedMagnitude() const { return velocity.Length(); }

	void Stop() {
		velocity = ZeroVector;
		isMoving = false;
	}

	void SetMovementMode(MovementMode mode) {
		movementMode = mode;
		if (mode == MovementMode::Stationary) {
			Stop();
		}
	}
};

} // namespace arclight
