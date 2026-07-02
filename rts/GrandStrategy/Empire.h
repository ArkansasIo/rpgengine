/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ARCLIGHT_GRAND_STRATEGY_EMPIRE_H
#define ARCLIGHT_GRAND_STRATEGY_EMPIRE_H

#include "Types.h"

#include <string>
#include <vector>

namespace ArcLight::GrandStrategy {

enum class GovernmentType : std::uint8_t {
	Democracy,
	Oligarchy,
	Monarchy,
	Corporate,
	HiveMind,
	MachineIntelligence,
	Imperial
};

enum class DiplomaticStance : std::uint8_t {
	Isolationist,
	Cooperative,
	Mercantile,
	Expansionist,
	Supremacist
};

struct Empire {
	EntityId id = 0;
	std::string name;
	GovernmentType government = GovernmentType::Democracy;
	DiplomaticStance stance = DiplomaticStance::Cooperative;
	ResourceLedger treasury;
	std::vector<EntityId> colonies;
	std::vector<EntityId> fleets;
	std::int32_t influence = 0;
	std::int32_t stability = 50;
	std::int32_t reputation = 0;
};

} // namespace ArcLight::GrandStrategy

#endif // ARCLIGHT_GRAND_STRATEGY_EMPIRE_H
