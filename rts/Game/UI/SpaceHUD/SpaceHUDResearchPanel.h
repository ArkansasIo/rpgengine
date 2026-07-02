/* ArcLight Engine - Space HUD Research Panel */

#pragma once

#include "SpaceHUDLayout.h"

#include <string>
#include <vector>

namespace arclight {

struct SpaceHUDResearchLine {
	std::string name;
	float progress = 0.0f;
	float total = 1.0f;
};

class SpaceHUDResearchPanel {
public:
	SpaceHUDPanel panel = {"Research", {0.82f, 0.10f, 0.18f, 0.36f}, true, true, 1.0f};
	std::vector<SpaceHUDResearchLine> queue;

	void Init() {
		queue = {
			{"Energy Technology", 1.0f, 4.2f},
			{"Hyperspace Drive", 0.0f, 12.0f},
		};
	}
};

} // namespace arclight
