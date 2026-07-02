/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ARCLIGHT_GRAND_STRATEGY_SIMULATION_H
#define ARCLIGHT_GRAND_STRATEGY_SIMULATION_H

#include "Economy.h"
#include "Empire.h"
#include "Fleet.h"
#include "Galaxy.h"

#include <vector>

namespace ArcLight::GrandStrategy {

struct SimulationState {
	Tick tick = 0;
	Galaxy galaxy;
	std::vector<Empire> empires;
	std::vector<Fleet> fleets;
};

struct SimulationConfig {
	std::uint32_t ticksPerMonth = 1800;
};

class Simulation {
public:
	explicit Simulation(SimulationConfig config = {}) : config(config) {}

	void Step(SimulationState& state) const
	{
		++state.tick;
		if (config.ticksPerMonth != 0 && (state.tick % config.ticksPerMonth) == 0) {
			for (Empire& empire: state.empires) {
				EconomyState economy;
				economy.stockpile = empire.treasury;
				EconomySystem::ApplyMonthlyTick(economy);
				empire.treasury = economy.stockpile;
			}
		}
	}

private:
	SimulationConfig config;
};

} // namespace ArcLight::GrandStrategy

#endif // ARCLIGHT_GRAND_STRATEGY_SIMULATION_H
