/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ARCLIGHT_GRAND_STRATEGY_FLEET_H
#define ARCLIGHT_GRAND_STRATEGY_FLEET_H

#include "Types.h"

#include <string>
#include <vector>

namespace ArcLight::GrandStrategy {

enum class ShipClass : std::uint8_t {
	Scout,
	Corvette,
	Frigate,
	Destroyer,
	Cruiser,
	Battleship,
	Carrier,
	Titan,
	Juggernaut,
	ColonyShip,
	Constructor,
	Transport
};

struct ShipDesign {
	EntityId id = 0;
	std::string name;
	ShipClass shipClass = ShipClass::Scout;
	std::uint32_t hull = 100;
	std::uint32_t shields = 0;
	std::uint32_t armor = 0;
	std::uint32_t power = 0;
	std::uint32_t firepower = 0;
};

struct Fleet {
	EntityId id = 0;
	EntityId ownerEmpireId = 0;
	EntityId currentSystemId = 0;
	EntityId targetSystemId = 0;
	FleetOrder order = FleetOrder::Idle;
	std::vector<EntityId> shipIds;
	std::uint32_t experience = 0;
};

} // namespace ArcLight::GrandStrategy

#endif // ARCLIGHT_GRAND_STRATEGY_FLEET_H
