/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Transform Component - position, rotation, scale
 * with local-to-world matrix caching and hierarchy support.
 */

#pragma once

#include "../Entity.h"
#include "../../System/float3.h"
#include "../../System/Matrix44f.h"

namespace arclight {

struct TransformComponent : public IComponent {
	float3 position = ZeroVector;
	float3 rotation = ZeroVector; // Euler angles (pitch, yaw, roll) in degrees
	float3 scale = float3(1.0f, 1.0f, 1.0f);

	EntityID parent = INVALID_ENTITY;

	mutable bool dirty = true;
	mutable Matrix44f cachedMatrix;

	const Matrix44f& GetLocalMatrix() const {
		if (dirty) {
			RebuildMatrix();
			dirty = false;
		}
		return cachedMatrix;
	}

	void SetPosition(const float3& pos) {
		position = pos;
		dirty = true;
	}

	void SetRotation(const float3& rot) {
		rotation = rot;
		dirty = true;
	}

	void SetScale(const float3& s) {
		scale = s;
		dirty = true;
	}

	void Translate(const float3& delta) {
		position += delta;
		dirty = true;
	}

	void Rotate(const float3& delta) {
		rotation += delta;
		dirty = true;
	}

	float3 GetForward() const {
		float yawRad = rotation.y * 3.14159265f / 180.0f;
		float pitchRad = rotation.x * 3.14159265f / 180.0f;
		return float3(
			-cosf(pitchRad) * sinf(yawRad),
			sinf(pitchRad),
			-cosf(pitchRad) * cosf(yawRad)
		).Normalize();
	}

	float3 GetRight() const {
		float yawRad = rotation.y * 3.14159265f / 180.0f;
		return float3(cosf(yawRad), 0.0f, -sinf(yawRad)).Normalize();
	}

	float3 GetUp() const {
		return GetRight().Cross(GetForward()).Normalize();
	}

private:
	void RebuildMatrix() const {
		// Build TRS matrix: T * R * S
		cachedMatrix.LoadIdentity();

		// Scale
		cachedMatrix.Scale(scale.x, scale.y, scale.z);

		// Rotation (Euler YXZ like UE5)
		float pitch = rotation.x * 3.14159265f / 180.0f;
		float yaw   = rotation.y * 3.14159265f / 180.0f;
		float roll  = rotation.z * 3.14159265f / 180.0f;

		float cp = cosf(pitch), sp = sinf(pitch);
		float cy = cosf(yaw),   sy = sinf(yaw);
		float cr = cosf(roll),  sr = sinf(roll);

		Matrix44f rotMat;
		rotMat.m[0]  = cy*cr + sy*sp*sr;
		rotMat.m[1]  = sr*cp;
		rotMat.m[2]  = -sy*cr + cy*sp*sr;
		rotMat.m[4]  = -cy*sr + sy*sp*cr;
		rotMat.m[5]  = cr*cp;
		rotMat.m[6]  = sy*sr + cy*sp*cr;
		rotMat.m[8]  = sy*cp;
		rotMat.m[9]  = -sp;
		rotMat.m[10] = cy*cp;

		cachedMatrix = cachedMatrix * rotMat;

		// Translation
		cachedMatrix.SetPos(position);
	}
};

} // namespace arclight
