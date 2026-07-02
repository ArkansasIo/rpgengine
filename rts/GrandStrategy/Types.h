/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ARCLIGHT_GRAND_STRATEGY_TYPES_H
#define ARCLIGHT_GRAND_STRATEGY_TYPES_H

#include <array>
#include <cstdint>
#include <string>

namespace ArcLight::GrandStrategy {

using EntityId = std::uint64_t;
using Tick = std::uint64_t;

struct FixedPoint {
	std::int64_t milli = 0;

	constexpr FixedPoint() = default;
	constexpr explicit FixedPoint(std::int64_t value) : milli(value) {}

	static constexpr FixedPoint FromUnits(std::int64_t value) { return FixedPoint(value * 1000); }
	static constexpr FixedPoint FromMilli(std::int64_t value) { return FixedPoint(value); }

	constexpr FixedPoint operator+(const FixedPoint rhs) const { return FixedPoint(milli + rhs.milli); }
	constexpr FixedPoint operator-(const FixedPoint rhs) const { return FixedPoint(milli - rhs.milli); }
	constexpr FixedPoint operator*(const std::int64_t rhs) const { return FixedPoint(milli * rhs); }

	FixedPoint& operator+=(const FixedPoint rhs) { milli += rhs.milli; return *this; }
	FixedPoint& operator-=(const FixedPoint rhs) { milli -= rhs.milli; return *this; }
};

enum class ResourceKind : std::uint8_t {
	Metal,
	Crystal,
	Deuterium,
	DarkMatter,
	Credits,
	Energy,
	Food,
	ExoticGas,
	RareCrystal,
	StrategicAlloy,
	Count
};

struct ResourceLedger {
	std::array<FixedPoint, static_cast<std::size_t>(ResourceKind::Count)> values{};

	FixedPoint& operator[](const ResourceKind kind)
	{
		return values[static_cast<std::size_t>(kind)];
	}

	const FixedPoint& operator[](const ResourceKind kind) const
	{
		return values[static_cast<std::size_t>(kind)];
	}

	ResourceLedger& operator+=(const ResourceLedger& rhs)
	{
		for (std::size_t i = 0; i < values.size(); ++i)
			values[i] += rhs.values[i];

		return *this;
	}
};

enum class StarClass : std::uint8_t {
	RedDwarf,
	YellowMainSequence,
	BlueGiant,
	WhiteDwarf,
	NeutronStar,
	BlackHole,
	Binary
};

enum class PlanetClass : std::uint8_t {
	Barren,
	Desert,
	Terran,
	Ocean,
	Arctic,
	Toxic,
	GasGiant,
	Molten,
	Relic,
	RingWorld
};

enum class FleetOrder : std::uint8_t {
	Idle,
	Survey,
	Colonize,
	Move,
	Patrol,
	Attack,
	Defend,
	Trade,
	Retreat
};

struct Vec3d {
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
};

} // namespace ArcLight::GrandStrategy

#endif // ARCLIGHT_GRAND_STRATEGY_TYPES_H
