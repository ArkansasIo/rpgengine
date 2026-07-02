/* ArcLight Engine - Space HUD Fleet Panel */

#pragma once

#include "SpaceHUDLayout.h"

#include <string>
#include <vector>

namespace arclight {

struct SpaceHUDFleetLine {
	std::string name;
	int count = 0;
};

class SpaceHUDFleetPanel {
public:
	SpaceHUDPanel panel = {"Fleet", {0.18f, 0.10f, 0.64f, 0.22f}, true, true, 1.0f};
	std::vector<SpaceHUDFleetLine> lines;

	void Init() {
		lines = {
			{"Battleship", 20},
			{"Destroyer", 30},
			{"Recycler", 20},
		};
	}
};

} // namespace arclight
