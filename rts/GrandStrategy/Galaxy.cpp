/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#include "Galaxy.h"

#include <algorithm>
#include <cmath>

namespace ArcLight::GrandStrategy {

namespace {
	constexpr double Pi = 3.14159265358979323846;

	EntityId MakeId(const std::uint64_t seed, const std::uint64_t type, const std::uint64_t index)
	{
		return (seed << 24) ^ (type << 56) ^ (index + 0x9e3779b97f4a7c15ull);
	}
}

DeterministicRng::DeterministicRng(const std::uint64_t seed)
	: state(seed + 0x9e3779b97f4a7c15ull)
{
}

std::uint64_t DeterministicRng::NextU64()
{
	std::uint64_t z = (state += 0x9e3779b97f4a7c15ull);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
	return z ^ (z >> 31);
}

std::uint32_t DeterministicRng::NextU32(const std::uint32_t upperExclusive)
{
	if (upperExclusive == 0)
		return 0;

	return static_cast<std::uint32_t>(NextU64() % upperExclusive);
}

double DeterministicRng::NextUnit()
{
	return static_cast<double>(NextU64() >> 11) * (1.0 / 9007199254740992.0);
}

bool DeterministicRng::Chance(const double probability)
{
	return NextUnit() < std::clamp(probability, 0.0, 1.0);
}

Galaxy GalaxyGenerator::Generate(const std::uint64_t seed, const GalaxyGenerationConfig& config) const
{
	DeterministicRng rng(seed);
	Galaxy galaxy;
	galaxy.id = MakeId(seed, 1, 0);
	galaxy.name = "ArcLight Galaxy";
	galaxy.seed = seed;
	galaxy.systems.reserve(config.systemCount);

	const std::uint32_t maxPlanets = std::max(config.minPlanetsPerSystem, config.maxPlanetsPerSystem);
	const std::uint32_t planetSpan = maxPlanets - config.minPlanetsPerSystem + 1;

	for (std::uint32_t systemIndex = 0; systemIndex < config.systemCount; ++systemIndex) {
		const double angle = rng.NextUnit() * Pi * 2.0;
		const double armOffset = std::sin(static_cast<double>(systemIndex) * 0.13) * 0.2;
		const double distance = std::sqrt(rng.NextUnit()) * config.radius;

		SolarSystem system;
		system.id = MakeId(seed, 2, systemIndex);
		system.name = "SYS-" + std::to_string(systemIndex + 1);
		system.position = {
			std::cos(angle + armOffset) * distance,
			(rng.NextUnit() - 0.5) * config.radius * 0.08,
			std::sin(angle + armOffset) * distance
		};
		system.starClass = PickStarClass(rng);
		system.hasWormhole = rng.Chance(0.015);
		system.hasAncientRuins = rng.Chance(0.05);

		const std::uint32_t planetCount = config.minPlanetsPerSystem + rng.NextU32(planetSpan);
		system.planets.reserve(planetCount);

		for (std::uint32_t planetIndex = 0; planetIndex < planetCount; ++planetIndex) {
			Planet planet;
			planet.id = MakeId(seed, 3 + systemIndex, planetIndex);
			planet.name = system.name + "-" + std::to_string(planetIndex + 1);
			planet.planetClass = PickPlanetClass(rng, system.starClass);
			planet.size = static_cast<std::uint16_t>(6 + rng.NextU32(20));
			planet.districts = static_cast<std::uint16_t>(planet.size / 2 + rng.NextU32(planet.size));
			planet.colonizable = planet.planetClass != PlanetClass::GasGiant && planet.planetClass != PlanetClass::Molten;
			planet.hasAnomaly = rng.Chance(0.08);
			planet.deposits = GenerateDeposits(rng, planet.planetClass);
			system.planets.push_back(std::move(planet));
		}

		galaxy.systems.push_back(std::move(system));
	}

	return galaxy;
}

StarClass GalaxyGenerator::PickStarClass(DeterministicRng& rng)
{
	const std::uint32_t roll = rng.NextU32(1000);
	if (roll < 360) return StarClass::RedDwarf;
	if (roll < 690) return StarClass::YellowMainSequence;
	if (roll < 800) return StarClass::WhiteDwarf;
	if (roll < 900) return StarClass::Binary;
	if (roll < 965) return StarClass::BlueGiant;
	if (roll < 990) return StarClass::NeutronStar;
	return StarClass::BlackHole;
}

PlanetClass GalaxyGenerator::PickPlanetClass(DeterministicRng& rng, const StarClass starClass)
{
	if (starClass == StarClass::BlackHole)
		return rng.Chance(0.75) ? PlanetClass::Barren : PlanetClass::Relic;

	const std::uint32_t roll = rng.NextU32(1000);
	if (roll < 180) return PlanetClass::Barren;
	if (roll < 300) return PlanetClass::Desert;
	if (roll < 430) return PlanetClass::Terran;
	if (roll < 540) return PlanetClass::Ocean;
	if (roll < 650) return PlanetClass::Arctic;
	if (roll < 760) return PlanetClass::Toxic;
	if (roll < 880) return PlanetClass::GasGiant;
	if (roll < 960) return PlanetClass::Molten;
	if (roll < 992) return PlanetClass::Relic;
	return PlanetClass::RingWorld;
}

ResourceLedger GalaxyGenerator::GenerateDeposits(DeterministicRng& rng, const PlanetClass planetClass)
{
	ResourceLedger deposits;
	deposits[ResourceKind::Metal] = FixedPoint::FromUnits(50 + rng.NextU32(450));
	deposits[ResourceKind::Crystal] = FixedPoint::FromUnits(20 + rng.NextU32(250));
	deposits[ResourceKind::Deuterium] = FixedPoint::FromUnits(10 + rng.NextU32(180));
	deposits[ResourceKind::Energy] = FixedPoint::FromUnits(100 + rng.NextU32(500));

	if (planetClass == PlanetClass::GasGiant)
		deposits[ResourceKind::ExoticGas] = FixedPoint::FromUnits(25 + rng.NextU32(150));

	if (planetClass == PlanetClass::Relic || planetClass == PlanetClass::RingWorld)
		deposits[ResourceKind::DarkMatter] = FixedPoint::FromUnits(5 + rng.NextU32(75));

	if (planetClass == PlanetClass::Terran || planetClass == PlanetClass::Ocean)
		deposits[ResourceKind::Food] = FixedPoint::FromUnits(100 + rng.NextU32(300));

	return deposits;
}

} // namespace ArcLight::GrandStrategy
