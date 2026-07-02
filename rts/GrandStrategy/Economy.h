/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ARCLIGHT_GRAND_STRATEGY_ECONOMY_H
#define ARCLIGHT_GRAND_STRATEGY_ECONOMY_H

#include "Types.h"

namespace ArcLight::GrandStrategy {

struct EconomyState {
	ResourceLedger stockpile;
	ResourceLedger monthlyIncome;
	ResourceLedger monthlyUpkeep;
};

class EconomySystem {
public:
	static void ApplyMonthlyTick(EconomyState& state)
	{
		for (std::size_t i = 0; i < state.stockpile.values.size(); ++i) {
			state.stockpile.values[i] += state.monthlyIncome.values[i];
			state.stockpile.values[i] -= state.monthlyUpkeep.values[i];
		}
	}
};

} // namespace ArcLight::GrandStrategy

#endif // ARCLIGHT_GRAND_STRATEGY_ECONOMY_H
