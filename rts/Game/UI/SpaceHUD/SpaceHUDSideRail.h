/* ArcLight Engine - Space HUD Side Rail */

#pragma once

#include "SpaceHUDLayout.h"

#include <string>
#include <vector>

namespace arclight {

struct SpaceHUDRailEntry {
	std::string id;
	std::string label;
	int unreadCount = 0;
	bool isSelected = false;
};

class SpaceHUDSideRail {
public:
	SpaceHUDPanel panel = {"SideRail", {0.0f, 0.10f, 0.18f, 0.80f}, true, true, 1.0f};
	std::vector<SpaceHUDRailEntry> entries;

	void Init() {
		entries = {
			{"overview", "Overview", 0, true},
			{"messages", "Messages", 3, false},
			{"events", "Events", 0, false},
			{"technology", "Technology", 0, false},
			{"fleet", "Fleet", 0, false},
			{"galaxy", "Galaxy", 0, false},
		};
	}
	
	void SyncForFocus(const std::string& resourceFocus, int focusCount = 0) {
		if (resourceFocus == "metal" || resourceFocus == "metal_income") {
			entries = {
				{"overview", "Overview", 0, true},
				{"shipyard", "Shipyard", 0, false},
				{"defense", "Defense", 0, false},
				{"market", "Market", 0, false},
				{"recycler", "Recycler", 0, false},
			};
		} else if (resourceFocus == "energy" || resourceFocus == "energy_income") {
			entries = {
				{"overview", "Overview", 0, true},
				{"research", "Research", 0, false},
				{"reactor", "Reactor", 0, false},
				{"shields", "Shields", 0, false},
				{"grid", "Power Grid", 0, false},
			};
		} else {
			entries = {
				{"overview", "Overview", 0, true},
				{"logistics", "Logistics", 0, false},
				{"routes", "Trade Routes", 0, false},
				{"storage", "Storage", 0, false},
				{"coordination", "Coordination", 0, false},
			};
		}

		if (!entries.empty())
			entries[0].unreadCount = focusCount;
	}
};

} // namespace arclight
