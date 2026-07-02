/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ARCLIGHT_GRAND_STRATEGY_GALAXY_H
#define ARCLIGHT_GRAND_STRATEGY_GALAXY_H

#include "Types.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ArcLight::GrandStrategy {

struct Planet {
	EntityId id = 0;
	std::string name;
	PlanetClass planetClass = PlanetClass::Barren;
	ResourceLedger deposits;
	std::uint16_t size = 0;
	std::uint16_t districts = 0;
	bool colonizable = false;
	bool hasAnomaly = false;
};

struct SolarSystem {
	EntityId id = 0;
	std::string name;
	Vec3d position;
	StarClass starClass = StarClass::YellowMainSequence;
	std::vector<Planet> planets;
	bool hasWormhole = false;
	bool hasAncientRuins = false;
};

struct Galaxy {
	EntityId id = 0;
	std::string name;
	std::uint64_t seed = 0;
	std::vector<SolarSystem> systems;
};

struct GalaxyGenerationConfig {
	std::uint32_t systemCount = 256;
	std::uint32_t minPlanetsPerSystem = 1;
	std::uint32_t maxPlanetsPerSystem = 12;
	double radius = 100000.0;
};

class DeterministicRng {
public:
	explicit DeterministicRng(std::uint64_t seed);

	std::uint64_t NextU64();
	std::uint32_t NextU32(std::uint32_t upperExclusive);
	double NextUnit();
	bool Chance(double probability);

private:
	std::uint64_t state;
};

class GalaxyGenerator {
public:
	Galaxy Generate(std::uint64_t seed, const GalaxyGenerationConfig& config) const;

private:
	static StarClass PickStarClass(DeterministicRng& rng);
	static PlanetClass PickPlanetClass(DeterministicRng& rng, StarClass starClass);
	static ResourceLedger GenerateDeposits(DeterministicRng& rng, PlanetClass planetClass);
};

} // namespace ArcLight::GrandStrategy

#endif // ARCLIGHT_GRAND_STRATEGY_GALAXY_H
