/* ArcLight Engine - Space HUD Top Bar */

#pragma once

#include "SpaceHUDLayout.h"

#include <string>
#include <vector>

namespace arclight {

struct SpaceHUDTopBarButton {
	std::string id;
	std::string label;
	bool isActive = false;
};

class SpaceHUDTopBar {
public:
	SpaceHUDPanel panel = {"TopBar", {0.0f, 0.0f, 1.0f, 0.10f}, true, true, 1.0f};
	std::vector<SpaceHUDResourceChip> chips;
	std::vector<SpaceHUDTopBarButton> buttons;

	void Init() {
		buttons = {
			{"overview", "Overview", true},
			{"messages", "Messages", false},
			{"events", "Events", false},
			{"technology", "Technology", false},
			{"shipyard", "Shipyard", false},
		};
	}

	void SyncFromLayout(const SpaceHUDLayout& layout) {
		chips = layout.resources;
	}
};

} // namespace arclight
