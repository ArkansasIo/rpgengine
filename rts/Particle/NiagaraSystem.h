/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5 Niagara-inspired GPU particle system.
 * Data-oriented particle simulation with emitters, modules, and renderers.
 */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <cmath>
#include <algorithm>

namespace arclight {

struct ParticleData {
	float3 position = ZeroVector;
	float3 velocity = ZeroVector;
	float3 acceleration = ZeroVector;
	float3 color = float3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
	float size = 1.0f;
	float rotation = 0.0f;
	float rotationSpeed = 0.0f;
	float lifetime = 1.0f;
	float age = 0.0f;
	float normalizedAge = 0.0f;
	int32_t particleID = 0;
	int32_t emitterID = 0;
	bool alive = true;

	void Update(float dt) {
		age += dt;
		normalizedAge = lifetime > 0.0f ? age / lifetime : 1.0f;
		if (age >= lifetime) { alive = false; return; }
		velocity += acceleration * dt;
		position += velocity * dt;
		rotation += rotationSpeed * dt;
	}
};

enum class EmitterModuleType { Spawn, Update, Render };

class IEmitterModule {
public:
	virtual ~IEmitterModule() = default;
	virtual EmitterModuleType GetType() const = 0;
	virtual void Execute(std::vector<ParticleData>& particles, float dt) = 0;
};

class SpawnRateModule : public IEmitterModule {
public:
	float rate = 10.0f;
	float accumulator = 0.0f;
	int maxParticles = 1000;

	EmitterModuleType GetType() const override { return EmitterModuleType::Spawn; }

	void Execute(std::vector<ParticleData>& particles, float dt) override {
		accumulator += rate * dt;
		int toSpawn = static_cast<int>(accumulator);
		accumulator -= toSpawn;
		for (int i = 0; i < toSpawn && static_cast<int>(particles.size()) < maxParticles; i++) {
			ParticleData p;
			p.alive = true;
			particles.push_back(p);
		}
	}
};

class BurstModule : public IEmitterModule {
public:
	int count = 50;
	bool hasTriggered = false;

	EmitterModuleType GetType() const override { return EmitterModuleType::Spawn; }

	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		if (hasTriggered) return;
		hasTriggered = true;
		for (int i = 0; i < count; i++) {
			ParticleData p;
			p.alive = true;
			particles.push_back(p);
		}
	}
};

class ParticleInitModule : public IEmitterModule {
public:
	float3 spawnBoxMin = float3(-10.0f, 0.0f, -10.0f);
	float3 spawnBoxMax = float3(10.0f, 20.0f, 10.0f);
	float3 initialVelocityMin = float3(-5.0f, 10.0f, -5.0f);
	float3 initialVelocityMax = float3(5.0f, 30.0f, 5.0f);
	float lifetimeMin = 0.5f;
	float lifetimeMax = 2.0f;
	float sizeMin = 0.1f;
	float sizeMax = 1.0f;

	EmitterModuleType GetType() const override { return EmitterModuleType::Spawn; }

	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		for (auto& p : particles) {
			if (p.age > 0.0f) continue;
			p.position = RandomInRange(spawnBoxMin, spawnBoxMax);
			p.velocity = RandomInRange(initialVelocityMin, initialVelocityMax);
			p.lifetime = RandomFloat(lifetimeMin, lifetimeMax);
			p.size = RandomFloat(sizeMin, sizeMax);
		}
	}

private:
	float3 RandomInRange(const float3& mn, const float3& mx) {
		return float3(RandomFloat(mn.x, mx.x), RandomFloat(mn.y, mx.y), RandomFloat(mn.z, mx.z));
	}
	float RandomFloat(float mn, float mx) {
		return mn + (mx - mn) * (static_cast<float>(rand()) / RAND_MAX);
	}
};

class GravityModule : public IEmitterModule {
public:
	float3 gravity = float3(0.0f, -9.81f, 0.0f);
	EmitterModuleType GetType() const override { return EmitterModuleType::Update; }
	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		for (auto& p : particles) { if (p.alive) p.acceleration = gravity; }
	}
};

class SizeOverLifetimeModule : public IEmitterModule {
public:
	float startSize = 1.0f;
	float endSize = 0.0f;
	EmitterModuleType GetType() const override { return EmitterModuleType::Update; }
	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		for (auto& p : particles) { if (p.alive) p.size = startSize + (endSize - startSize) * p.normalizedAge; }
	}
};

class ColorOverLifetimeModule : public IEmitterModule {
public:
	float3 startColor = float3(1.0f, 0.8f, 0.2f);
	float3 endColor = float3(1.0f, 0.1f, 0.0f);
	EmitterModuleType GetType() const override { return EmitterModuleType::Update; }
	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		for (auto& p : particles) {
			if (p.alive) p.color = startColor + (endColor - startColor) * p.normalizedAge;
		}
	}
};

class AlphaOverLifetimeModule : public IEmitterModule {
public:
	float startAlpha = 1.0f;
	float endAlpha = 0.0f;
	EmitterModuleType GetType() const override { return EmitterModuleType::Update; }
	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		for (auto& p : particles) {
			if (p.alive) p.alpha = startAlpha + (endAlpha - startAlpha) * p.normalizedAge;
		}
	}
};

class CurlNoiseModule : public IEmitterModule {
public:
	float noiseStrength = 50.0f;
	float noiseFrequency = 0.1f;
	float noiseSpeed = 1.0f;
	EmitterModuleType GetType() const override { return EmitterModuleType::Update; }
	void Execute(std::vector<ParticleData>& particles, float dt) override {
		for (auto& p : particles) {
			if (!p.alive) continue;
			float t = p.age * noiseSpeed;
			float nx = sinf(p.position.x * noiseFrequency + t) * cosf(p.position.z * noiseFrequency);
			float ny = cosf(p.position.y * noiseFrequency + t) * 0.5f;
			float nz = sinf(p.position.z * noiseFrequency + t) * sinf(p.position.x * noiseFrequency);
			p.velocity += float3(nx, ny, nz) * noiseStrength * dt;
		}
	}
};

class KillDeadModule : public IEmitterModule {
public:
	EmitterModuleType GetType() const override { return EmitterModuleType::Update; }
	void Execute(std::vector<ParticleData>& particles, float /*dt*/) override {
		particles.erase(
			std::remove_if(particles.begin(), particles.end(),
				[](const ParticleData& p) { return !p.alive; }),
			particles.end()
		);
	}
};

class IParticleRenderer {
public:
	virtual ~IParticleRenderer() = default;
	virtual void Render(const std::vector<ParticleData>& /*particles*/) {}
};

class BillboardRenderer : public IParticleRenderer {
public:
	bool sortBackToFront = true;
	void Render(const std::vector<ParticleData>& /*particles*/) override {}
};

class NiagaraSystem {
public:
	std::string systemName;
	std::vector<std::unique_ptr<IEmitterModule>> modules;
	std::vector<ParticleData> particles;
	std::unique_ptr<IParticleRenderer> renderer;
	bool isActive = true;
	float simulationSpeed = 1.0f;

	void AddModule(std::unique_ptr<IEmitterModule> module) {
		modules.push_back(std::move(module));
	}

	void Update(float dt) {
		if (!isActive) return;
		float scaledDt = dt * simulationSpeed;
		for (auto& mod : modules) {
			if (mod->GetType() == EmitterModuleType::Spawn)
				mod->Execute(particles, scaledDt);
		}
		for (auto& mod : modules) {
			if (mod->GetType() == EmitterModuleType::Update)
				mod->Execute(particles, scaledDt);
		}
		for (auto& p : particles) {
			if (p.alive) p.Update(scaledDt);
		}
	}

	void Render() { if (renderer) renderer->Render(particles); }

	void Reset() {
		particles.clear();
		for (auto& mod : modules) {
			auto* burst = dynamic_cast<BurstModule*>(mod.get());
			if (burst) burst->hasTriggered = false;
		}
	}

	size_t GetActiveParticleCount() const {
		size_t count = 0;
		for (auto& p : particles) { if (p.alive) count++; }
		return count;
	}
};

} // namespace arclight
