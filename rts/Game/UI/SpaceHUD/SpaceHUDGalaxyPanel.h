/* ArcLight Engine - Space HUD Galaxy Panel */

#pragma once

#include "SpaceHUDLayout.h"

#include <string>
#include <vector>

namespace arclight {

class SpaceHUDGalaxyPanel {
public:
	SpaceHUDPanel panel = {"Galaxy", {0.18f, 0.10f, 0.64f, 0.74f}, true, false, 1.0f};
	std::vector<SpaceHUDMapMarker> markers;
	std::vector<SpaceHUDMapMarker> visibleMarkers;
	bool showLabels = true;
	bool showRoutes = true;

	void SyncFromLayout(const SpaceHUDLayout& layout, const std::string& resourceFocus) {
		markers = layout.mapMarkers;
		visibleMarkers.clear();

		const auto matchesFocus = [&](const SpaceHUDMapMarker& marker) {
			if (resourceFocus == "metal" || resourceFocus == "metal_income")
				return (marker.type == "shipyard" || marker.type == "factory" || marker.type == "defense" || marker.type == "logistics");
			if (resourceFocus == "energy" || resourceFocus == "energy_income")
				return (marker.type == "power" || marker.type == "research" || marker.type == "shield" || marker.type == "reactor");
			return (marker.type == "storage" || marker.type == "economy" || marker.type == "trade" || marker.type == "supply");
		};

		for (const auto& marker : markers) {
			if (matchesFocus(marker))
				visibleMarkers.push_back(marker);
		}

		if (visibleMarkers.empty())
			visibleMarkers = markers;
	}
};

} // namespace arclight
