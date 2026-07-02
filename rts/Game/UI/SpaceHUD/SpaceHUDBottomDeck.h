/* ArcLight Engine - Space HUD Bottom Deck */

#pragma once

#include "SpaceHUDLayout.h"

#include <string>
#include <vector>

namespace arclight {

struct SpaceHUDBuildTab {
	std::string id;
	std::string label;
	bool isActive = false;
};

class SpaceHUDBottomDeck {
public:
	SpaceHUDPanel panel = {"BottomDeck", {0.18f, 0.84f, 0.64f, 0.16f}, true, true, 1.0f};
	std::vector<SpaceHUDBuildTab> tabs;
	std::vector<SpaceHUDQueueItem> queue;

	void Init() {
		tabs = {
			{"buildings", "Buildings", true},
			{"ships", "Ships", false},
			{"defense", "Defense", false},
			{"structures", "Structures", false},
			{"research", "Research", false},
		};
	}

	void SyncFromLayout(const SpaceHUDLayout& layout) {
		queue = layout.buildQueue;
	}
};

} // namespace arclight
