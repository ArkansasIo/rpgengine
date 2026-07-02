/* ArcLight Engine - Space Strategy HUD Layout
 * Developer: Stephen
 * Layout scaffold for a modern space strategy interface:
 * top resource strip, left command rail, right status stack,
 * bottom ship/build deck, and a central viewport overlay.
 */

#pragma once

#include "SpaceHUDTheme.h"
#include "System/float3.h"

#include "Sim/Units/CommandAI/Command.h"

#include <algorithm>
#include <string>
#include <vector>

namespace arclight {

struct SpaceHUDRect {
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
};

struct SpaceHUDResourceChip {
	std::string id;
	std::string label;
	float value = 0.0f;
	float perSecond = 0.0f;
	SpaceHUDColor tint;
	bool isCritical = false;
};

struct SpaceHUDQueueItem {
	std::string id;
	std::string label;
	int remainingSeconds = 0;
	int count = 1;
	bool isActive = true;
};

struct SpaceHUDMapMarker {
	std::string id;
	std::string name;
	float3 position = ZeroVector;
	std::string type;
	int ownerTeam = -1;
	float size = 1.0f;
	bool isSelected = false;
};

struct SpaceHUDCommandCard {
	int commandID = 0;
	std::string name;
	std::string tooltip;
	bool isDisabled = false;
	bool isBuildCommand = false;
};

struct SpaceHUDSelectionSummary {
	int selectedUnits = 0;
	int activeCommandPage = 0;
	int availableCommands = 0;
	bool hasSelection = false;
};

struct SpaceHUDResourceSnapshot {
	float metal = 0.0f;
	float metalIncome = 0.0f;
	float metalStorage = 0.0f;
	float energy = 0.0f;
	float energyIncome = 0.0f;
	float energyStorage = 0.0f;
};

struct SpaceHUDPanel {
	std::string title;
	SpaceHUDRect bounds;
	bool isVisible = true;
	bool isPinned = false;
	float opacity = 1.0f;
};

class SpaceHUDLayout {
public:
	SpaceHUDTheme theme;
	SpaceHUDPanel topStrip = {"Resources", {0.0f, 0.0f, 1.0f, 0.10f}, true, true, 1.0f};
	SpaceHUDPanel leftRail = {"Empire", {0.0f, 0.10f, 0.18f, 0.80f}, true, true, 1.0f};
	SpaceHUDPanel rightStack = {"Operations", {0.82f, 0.10f, 0.18f, 0.80f}, true, true, 1.0f};
	SpaceHUDPanel bottomDeck = {"Construction", {0.18f, 0.84f, 0.64f, 0.16f}, true, true, 1.0f};
	SpaceHUDPanel centerViewport = {"Main View", {0.18f, 0.10f, 0.64f, 0.74f}, true, false, 1.0f};

	std::vector<SpaceHUDResourceChip> resources;
	std::vector<SpaceHUDQueueItem> buildQueue;
	std::vector<SpaceHUDCommandCard> commandCards;
	std::vector<SpaceHUDMapMarker> mapMarkers;
	SpaceHUDSelectionSummary selection;
	SpaceHUDResourceSnapshot resourceSnapshot;
	std::vector<std::string> leftNavEntries;
	std::vector<std::string> rightNavEntries;
	std::vector<std::string> bottomTabs;

	void InitDefaultLayout() {
		resources = {
			{"metal", "Metal", 362.0f, 12.5f, theme.accent, false},
			{"energy", "Energy", 188.0f, 6.2f, theme.accentAlt, false},
			{"metal_income", "Metal /s", 12.5f, 0.0f, theme.success, false},
			{"energy_income", "Energy /s", 6.2f, 0.0f, theme.warning, false},
			{"storage", "Storage", 480.0f, 0.0f, theme.danger, true},
		};

		buildQueue = {
			{"shipyard_battleship", "Battleship", 742, 1, true},
			{"shipyard_destroyer", "Destroyer", 458, 1, true},
			{"shipyard_recycler", "Recycler", 102, 1, true},
		};

		leftNavEntries = {
			"Overview",
			"Messages",
			"Events",
			"Technology",
			"Shipyard",
			"Defense",
			"Fleet",
			"Galaxy",
			"Alliance",
			"Recruitment",
			"Market",
		};

		rightNavEntries = {
			"Events",
			"Build Queue",
			"Fleet",
			"Research",
		};

		bottomTabs = {
			"Buildings",
			"Ships",
			"Defense",
			"Structures",
			"Research",
		};
	}

	void Resize(float screenW, float screenH) {
		const float safeW = std::max(screenW, 1.0f);
		const float safeH = std::max(screenH, 1.0f);

		topStrip.bounds = {0.0f, 0.0f, safeW, safeH * 0.10f};
		leftRail.bounds = {0.0f, safeH * 0.10f, safeW * 0.18f, safeH * 0.74f};
		rightStack.bounds = {safeW * 0.82f, safeH * 0.10f, safeW * 0.18f, safeH * 0.74f};
		bottomDeck.bounds = {safeW * 0.18f, safeH * 0.84f, safeW * 0.64f, safeH * 0.16f};
		centerViewport.bounds = {safeW * 0.18f, safeH * 0.10f, safeW * 0.64f, safeH * 0.74f};
	}

	SpaceHUDRect GetMiniMapRect() const {
		return {0.0f, 0.84f, 0.18f, 0.16f};
	}

	SpaceHUDRect GetChatRect() const {
		return {0.18f, 0.84f, 0.28f, 0.16f};
	}

	SpaceHUDRect GetCommandDeckRect() const {
		return bottomDeck.bounds;
	}

	SpaceHUDRect GetResourceStripRect() const {
		return topStrip.bounds;
	}

	void SetResourceSnapshot(const SpaceHUDResourceSnapshot& snapshot) {
		resourceSnapshot = snapshot;
		if (resources.size() >= 5) {
			resources[0].value = snapshot.metal;
			resources[0].perSecond = snapshot.metalIncome;
			resources[1].value = snapshot.energy;
			resources[1].perSecond = snapshot.energyIncome;
			resources[2].value = snapshot.metalIncome;
			resources[3].value = snapshot.energyIncome;
			resources[4].value = snapshot.metalStorage + snapshot.energyStorage;
		}
	}

	void SetSelectionSummary(const SpaceHUDSelectionSummary& summary) {
		selection = summary;
	}

	void SetCommandCards(const std::vector<SpaceHUDCommandCard>& cards) {
		commandCards = cards;
		buildQueue.clear();
		buildQueue.reserve(cards.size());
		for (const auto& card : cards) {
			SpaceHUDQueueItem item;
			item.id = std::to_string(card.commandID);
			item.label = card.name;
			item.remainingSeconds = 0;
			item.count = 1;
			item.isActive = !card.isDisabled;
			buildQueue.push_back(item);
		}
	}
};

} // namespace arclight
