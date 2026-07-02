/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5 Chaos-inspired destruction system.
 * Supports fracture, rigid body debris, and constraint-based destruction.
 */

#pragma once

#include "../ECS/Components/Types.h"
#include <vector>
#include <memory>
#include <functional>

namespace arclight {

enum class EFractureMode {
	Voronoi,      // Voronoi-based fracture (realistic shattering)
	Planar,       // Planar cut
	Radial,       // Radial explosion pattern
	Clustered,    // Clustered chunks
};

struct FractureChunk {
	float3 centerOfMass = ZeroVector;
	float3 velocity = ZeroVector;
	float3 angularVelocity = ZeroVector;
	float3 boundingBoxMin = ZeroVector;
	float3 boundingBoxMax = ZeroVector;
	float mass = 1.0f;
	float lifeTime = 5.0f;
	float age = 0.0f;
	bool isActive = true;
	int parentIndex = -1; // -1 = root
	int depthLevel = 0;  // recursion depth
	std::vector<float3> vertices;
	std::vector<int> indices;
};

struct DestructionParams {
	EFractureMode mode = EFractureMode::Voronoi;
	int chunkCount = 8;
	float damageThreshold = 50.0f;
	float impactRadius = 100.0f;
	float impulseStrength = 500.0f;
	float debrisLifeTime = 5.0f;
	float debrisGravity = -9.81f;
	bool applyDamageToChunks = true;
	int maxRecursionDepth = 2;
	float recursionDamageReduction = 0.5f;
};

struct DestructionEvent {
	float3 impactPoint = ZeroVector;
	float3 impactDirection = ZeroVector;
	float damage = 0.0f;
	float radius = 0.0f;
};

using DestructionCallback = std::function<void(const std::vector<FractureChunk>&)>;

class DestructionSystem {
public:
	void SetParams(const DestructionParams& p) { this->params = p; }

	/**
	 * Fracture an object at a point, returning the resulting chunks.
	 * Simulates Voronoi or other fracture patterns.
	 */
	std::vector<FractureChunk> Fracture(
		const float3& center,
		const float3& size,
		const DestructionEvent& event
	) {
		std::vector<FractureChunk> chunks;

		if (event.damage < params.damageThreshold) return chunks;

		int numChunks = CalculateChunkCount(event.damage);
		float forceMultiplier = event.damage / params.damageThreshold;

		switch (params.mode) {
			case EFractureMode::Voronoi:
				chunks = GenerateVoronoiFracture(center, size, numChunks);
				break;
			case EFractureMode::Planar:
				chunks = GeneratePlanarFracture(center, size, numChunks);
				break;
			case EFractureMode::Radial:
				chunks = GenerateRadialFracture(center, size, numChunks, event.impactDirection);
				break;
			case EFractureMode::Clustered:
				chunks = GenerateClusteredFracture(center, size, numChunks);
				break;
		}

		// Apply impulse forces
		ApplyImpactForce(chunks, event.impactPoint, event.impactDirection, forceMultiplier);

		// Recursive sub-fracture
		if (params.maxRecursionDepth > 0) {
			ApplyRecursiveFracture(chunks, event, 1);
		}

		for (auto& chunk : chunks) {
			chunk.lifeTime = params.debrisLifeTime;
		}
		if (onChunkCreated) onChunkCreated(chunks);

		return chunks;
	}

	void Update(float dt, std::vector<FractureChunk>& chunks) {
		for (auto& chunk : chunks) {
			if (!chunk.isActive) continue;

			chunk.age += dt;
			if (chunk.age >= chunk.lifeTime) {
				chunk.isActive = false;
				continue;
			}

			// Apply gravity
			chunk.velocity.y += params.debrisGravity * dt;

			// Update position
			chunk.centerOfMass += chunk.velocity * dt;

			// Update rotation (simplified)
			chunk.angularVelocity *= 0.99f; // angular damping

			// Ground collision
			if (chunk.centerOfMass.y < 0.0f) {
				chunk.centerOfMass.y = 0.0f;
				chunk.velocity.y *= -0.3f; // bounce
				chunk.velocity.x *= 0.8f;
				chunk.velocity.z *= 0.8f;
				chunk.angularVelocity *= 0.7f;
			}
		}

		// Cleanup dead chunks
		chunks.erase(
			std::remove_if(chunks.begin(), chunks.end(),
				[](const FractureChunk& c) { return !c.isActive; }
			),
			chunks.end()
		);
	}

	/**
	 * Calculate if an object should break based on accumulated damage.
	 */
	bool ShouldBreak(float accumulatedDamage, float structuralHealth) const {
		return accumulatedDamage >= params.damageThreshold * (structuralHealth / 100.0f);
	}

	DestructionCallback onChunkCreated;

private:
	DestructionParams params;

	int CalculateChunkCount(float damage) const {
		float ratio = damage / params.damageThreshold;
		return std::max(2, static_cast<int>(params.chunkCount * ratio));
	}

	std::vector<FractureChunk> GenerateVoronoiFracture(
		const float3& center, const float3& size, int count
	) {
		std::vector<FractureChunk> chunks;

		// Generate Voronoi seed points
		std::vector<float3> seeds;
		for (int i = 0; i < count; i++) {
			float3 seed;
			seed.x = center.x + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.x;
			seed.y = center.y + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.y;
			seed.z = center.z + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.z;
			seeds.push_back(seed);
		}

		// Create chunks from seeds (simplified Voronoi)
		for (int i = 0; i < count; i++) {
			FractureChunk chunk;
			chunk.centerOfMass = seeds[i];
			chunk.mass = size.x * size.y * size.z / count;

			// Bounding box around seed
			float chunkSize = std::cbrtf(size.x * size.y * size.z / count);
			chunk.boundingBoxMin = seeds[i] - float3(chunkSize * 0.5f, chunkSize * 0.5f, chunkSize * 0.5f);
			chunk.boundingBoxMax = seeds[i] + float3(chunkSize * 0.5f, chunkSize * 0.5f, chunkSize * 0.5f);

			// Generate simple box geometry for each chunk
			GenerateBoxVertices(chunk, chunkSize * 0.5f);

			chunks.push_back(chunk);
		}

		return chunks;
	}

	std::vector<FractureChunk> GeneratePlanarFracture(
		const float3& center, const float3& size, int count
	) {
		std::vector<FractureChunk> chunks;
		// Split along one axis
		for (int i = 0; i < count; i++) {
			FractureChunk chunk;
			float t = static_cast<float>(i) / count;
			chunk.centerOfMass = center + float3(0.0f, size.y * (t - 0.5f), 0.0f);
			chunk.mass = size.x * (size.y / count) * size.z;
			float halfH = size.y / count * 0.5f;
			chunk.boundingBoxMin = chunk.centerOfMass - float3(size.x * 0.5f, halfH, size.z * 0.5f);
			chunk.boundingBoxMax = chunk.centerOfMass + float3(size.x * 0.5f, halfH, size.z * 0.5f);
			GenerateBoxVertices(chunk, halfH);
			chunks.push_back(chunk);
		}
		return chunks;
	}

	std::vector<FractureChunk> GenerateRadialFracture(
		const float3& center, const float3& size, int count, const float3& /*direction*/
	) {
		std::vector<FractureChunk> chunks;
		for (int i = 0; i < count; i++) {
			FractureChunk chunk;
			float angle = (static_cast<float>(i) / count) * 3.14159265f * 2.0f;
			float radius = std::cbrtf(size.x * size.y * size.z / count) * 0.5f;
			chunk.centerOfMass = center + float3(
				cosf(angle) * radius,
				(static_cast<float>(rand()) / RAND_MAX - 0.3f) * radius,
				sinf(angle) * radius
			);
			chunk.mass = size.x * size.y * size.z / count;
			float halfSize = radius * 0.5f;
			chunk.boundingBoxMin = chunk.centerOfMass - float3(halfSize, halfSize, halfSize);
			chunk.boundingBoxMax = chunk.centerOfMass + float3(halfSize, halfSize, halfSize);
			GenerateBoxVertices(chunk, halfSize);
			chunks.push_back(chunk);
		}
		return chunks;
	}

	std::vector<FractureChunk> GenerateClusteredFracture(
		const float3& center, const float3& size, int count
	) {
		// Clustered = larger chunks with smaller sub-chunks
		std::vector<FractureChunk> chunks;
		int clusters = std::max(2, count / 3);
		for (int c = 0; c < clusters; c++) {
			float3 clusterCenter = center + float3(
				(static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.x,
				(static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.y,
				(static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.z
			);
			int subChunks = count / clusters;
			for (int i = 0; i < subChunks; i++) {
				FractureChunk chunk;
				chunk.centerOfMass = clusterCenter + float3(
					(static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.x * 0.3f,
					(static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.y * 0.3f,
					(static_cast<float>(rand()) / RAND_MAX - 0.5f) * size.z * 0.3f
				);
				chunk.mass = size.x * size.y * size.z / count;
				float halfSize = std::cbrtf(chunk.mass) * 0.3f;
				chunk.boundingBoxMin = chunk.centerOfMass - float3(halfSize, halfSize, halfSize);
				chunk.boundingBoxMax = chunk.centerOfMass + float3(halfSize, halfSize, halfSize);
				GenerateBoxVertices(chunk, halfSize);
				chunks.push_back(chunk);
			}
		}
		return chunks;
	}

	void GenerateBoxVertices(FractureChunk& chunk, float halfSize) {
		float3 center = chunk.centerOfMass;
		// 8 corners of a box
		float3 corners[8] = {
			center + float3(-halfSize, -halfSize, -halfSize),
			center + float3( halfSize, -halfSize, -halfSize),
			center + float3( halfSize,  halfSize, -halfSize),
			center + float3(-halfSize,  halfSize, -halfSize),
			center + float3(-halfSize, -halfSize,  halfSize),
			center + float3( halfSize, -halfSize,  halfSize),
			center + float3( halfSize,  halfSize,  halfSize),
			center + float3(-halfSize,  halfSize,  halfSize),
		};
		for (auto& c : corners) chunk.vertices.push_back(c);

		// 12 triangles (2 per face)
		int idx[] = {
			0,1,2, 0,2,3, 4,6,5, 4,7,6,
			0,4,5, 0,5,1, 2,6,7, 2,7,3,
			0,3,7, 0,7,4, 1,5,6, 1,6,2
		};
		for (int i : idx) chunk.indices.push_back(i);
	}

	void ApplyImpactForce(
		std::vector<FractureChunk>& chunks,
		const float3& impactPoint,
		const float3& direction,
		float forceMultiplier
	) {
		for (auto& chunk : chunks) {
			float3 toChunk = chunk.centerOfMass - impactPoint;
			float distance = toChunk.Length();
			if (distance < 0.001f) distance = 0.001f;

			float falloff = 1.0f / (1.0f + distance * 0.01f);
			float3 impulse = direction * params.impulseStrength * forceMultiplier * falloff / chunk.mass;

			// Add some radial push
			float3 radial = toChunk.Normalize() * params.impulseStrength * 0.3f * forceMultiplier / chunk.mass;

			chunk.velocity += impulse + radial;
			chunk.angularVelocity += float3(
				(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 10.0f,
				(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 10.0f,
				(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 10.0f
			) * forceMultiplier;
		}
	}

	void ApplyRecursiveFracture(
		std::vector<FractureChunk>& chunks,
		const DestructionEvent& event,
		int currentDepth
	) {
		if (currentDepth >= params.maxRecursionDepth) return;

		std::vector<FractureChunk> newChunks;
		for (auto& chunk : chunks) {
			float localDamage = event.damage * params.recursionDamageReduction * currentDepth;
			if (localDamage > params.damageThreshold && chunk.mass > 1.0f) {
				float3 subSize = chunk.boundingBoxMax - chunk.boundingBoxMin;
				DestructionEvent subEvent = event;
				subEvent.damage = localDamage;
				subEvent.radius = params.impactRadius * 0.5f;
				auto subChunks = Fracture(chunk.centerOfMass, subSize, subEvent);
				for (auto& sc : subChunks) {
					sc.depthLevel = currentDepth;
					sc.parentIndex = static_cast<int>(&chunk - &chunks[0]);
					newChunks.push_back(sc);
				}
				chunk.isActive = false;
			}
		}
		chunks.insert(chunks.end(), newChunks.begin(), newChunks.end());
	}
};

} // namespace arclight
