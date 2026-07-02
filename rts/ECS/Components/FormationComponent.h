/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Formation System for RTS unit control.
 * Supports classic RTS formations plus advanced UE5-style
 * spatial formation with dynamic adjustment.
 */

#pragma once

#include "../Entity.h"
#include "../../System/float3.h"
#include <vector>
#include <string>

namespace arclight {

enum class FormationType {
	Line,           // single rank
	Column,         // single file
	Wedge,          // V-shape
	Diamond,        // diamond shape
	Circle,         // circular formation
	Box,            // rectangular grid
	Scatter,        // loose spread
	Custom,         // user-defined positions
};

struct FormationSlot {
	int slotIndex = 0;
	float3 localOffset = ZeroVector; // offset from formation leader
	EntityID assignedEntity = INVALID_ENTITY;
	float spacing = 20.0f;
};

class Formation {
public:
	FormationType type = FormationType::Line;
	float3 leaderPosition = ZeroVector;
	float leaderHeading = 0.0f;
	float defaultSpacing = 20.0f;
	int maxSlots = 12;

	std::vector<FormationSlot> slots;

	void Initialize(FormationType newType, int unitCount, float spacing = 20.0f) {
		type = newType;
		defaultSpacing = spacing;
		slots.clear();
		slots.reserve(unitCount);

		for (int i = 0; i < unitCount; i++) {
			FormationSlot slot;
			slot.slotIndex = i;
			slot.spacing = spacing;
			slot.localOffset = CalculateSlotOffset(i, unitCount, spacing);
			slots.push_back(slot);
		}
	}

	float3 GetSlotWorldPosition(int slotIndex) const {
		if (slotIndex < 0 || slotIndex >= static_cast<int>(slots.size())) {
			return leaderPosition;
		}

		float3 offset = slots[slotIndex].localOffset;

		// Rotate offset by leader heading
		float cosH = cosf(leaderHeading * 3.14159265f / 180.0f);
		float sinH = sinf(leaderHeading * 3.14159265f / 180.0f);

		float3 rotated;
		rotated.x = offset.x * cosH - offset.z * sinH;
		rotated.y = offset.y;
		rotated.z = offset.x * sinH + offset.z * cosH;

		return leaderPosition + rotated;
	}

	void UpdateLeader(const float3& position, float heading) {
		leaderPosition = position;
		leaderHeading = heading;
	}

	// Dynamically adjust formation based on terrain
	void AdjustForTerrain(const std::function<float3(const float3&)>& getGroundPos) {
		for (auto& slot : slots) {
			float3 worldPos = GetSlotWorldPosition(slot.slotIndex);
			float3 groundPos = getGroundPos(worldPos);
			slot.localOffset.y = groundPos.y - leaderPosition.y;
		}
	}

	// Close formation when approaching enemy
	void Compress(float factor) {
		for (auto& slot : slots) {
			slot.localOffset = slot.localOffset * factor;
		}
	}

	void Expand() {
		for (auto& slot : slots) {
			slot.localOffset = CalculateSlotOffset(
				slot.slotIndex, static_cast<int>(slots.size()), defaultSpacing
			);
		}
	}

private:
	float3 CalculateSlotOffset(int index, int totalCount, float spacing) const {
		if (totalCount <= 0) return ZeroVector;

		switch (type) {
			case FormationType::Line: {
				int row = index / 3;
				int col = index % 3;
				int centerCol = 1;
				return float3(
					(col - centerCol) * spacing,
					0.0f,
					row * spacing
				);
			}

			case FormationType::Column: {
				return float3(0.0f, 0.0f, index * spacing);
			}

			case FormationType::Wedge: {
				int rank = static_cast<int>(sqrtf(static_cast<float>(index)));
				int posInRank = index - rank * rank;
				int halfWidth = rank;
				return float3(
					(posInRank - halfWidth) * spacing,
					0.0f,
					rank * spacing
				);
			}

			case FormationType::Diamond: {
				int halfCount = totalCount / 2;
				if (index < halfCount) {
					// Front half: expanding
					return float3(0.0f, 0.0f, -index * spacing);
				} else {
					// Back half: contracting
					return float3(0.0f, 0.0f, -(totalCount - index) * spacing);
				}
			}

			case FormationType::Circle: {
				float angle = (static_cast<float>(index) / totalCount) * 3.14159265f * 2.0f;
				float radius = spacing * totalCount / (3.14159265f * 2.0f);
				return float3(
					cosf(angle) * radius,
					0.0f,
					sinf(angle) * radius
				);
			}

			case FormationType::Box: {
				int cols = static_cast<int>(ceilf(sqrtf(static_cast<float>(totalCount))));
				int row = index / cols;
				int col = index % cols;
				int centerCol = cols / 2;
				int centerRow = totalCount / cols / 2;
				return float3(
					(col - centerCol) * spacing,
					0.0f,
					(row - centerRow) * spacing
				);
			}

			case FormationType::Scatter: {
				float angle = (static_cast<float>(rand()) / RAND_MAX) * 3.14159265f * 2.0f;
				float dist = (static_cast<float>(rand()) / RAND_MAX) * spacing * 2.0f;
				return float3(
					cosf(angle) * dist,
					0.0f,
					sinf(angle) * dist
				);
			}

			default:
				return float3(
					(index % 3 - 1) * spacing,
					0.0f,
					(index / 3) * spacing
				);
		}
	}
};

} // namespace arclight
