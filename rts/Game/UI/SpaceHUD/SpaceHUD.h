/* ArcLight Engine - Space Strategy HUD Scaffold
 * Developer: Stephen
 * HUD orchestration stub for a space strategy UI inspired by the provided
 * reference: top resources, left navigation, center tactical view, right
 * action stack, bottom ship/build deck, and compact command panels.
 */

#pragma once

#include "SpaceHUDLayout.h"
#include "SpaceHUDBottomDeck.h"
#include "SpaceHUDFleetPanel.h"
#include "SpaceHUDGalaxyPanel.h"
#include "SpaceHUDResearchPanel.h"
#include "SpaceHUDSideRail.h"
#include "SpaceHUDTopBar.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <cctype>

namespace arclight {

class SpaceHUD {
public:
	SpaceHUDLayout layout;
	SpaceHUDTopBar topBar;
	SpaceHUDSideRail sideRail;
	SpaceHUDBottomDeck bottomDeck;
	SpaceHUDGalaxyPanel galaxyPanel;
	SpaceHUDFleetPanel fleetPanel;
	SpaceHUDResearchPanel researchPanel;
	std::string activeSelection = "Terra Prime";
	std::string activeStatus = "Production online";
	std::string activeTab = "overview";
	std::string activeResource = "metal";
	float resourcePulse = 0.0f;
	float zoomLevel = 1.0f;
	bool showGalaxyOverlay = true;
	bool showBuildQueue = true;
	bool showFleetPanels = true;
	std::vector<SpaceHUDQueueItem> focusedQueue;

	void Init() {
		layout.InitDefaultLayout();
		layout.Resize(1920.0f, 1080.0f);
		topBar.Init();
		sideRail.Init();
		bottomDeck.Init();
		fleetPanel.Init();
		researchPanel.Init();
		RefreshFocusedViews();
		SetActiveTab(activeTab);
	}

	void Update(float /*dt*/) {
		resourcePulse = std::fmod(resourcePulse + 0.07f, 6.2831853f);
	}

	void SyncLiveState(const SpaceHUDResourceSnapshot& resources,
					  const SpaceHUDSelectionSummary& selectionSummary,
					  const std::vector<SpaceHUDCommandCard>& commandList) {
		layout.SetResourceSnapshot(resources);
		layout.SetSelectionSummary(selectionSummary);
		layout.SetCommandCards(commandList);
		layout.selection.activeCommandPage = selectionSummary.activeCommandPage;
		topBar.SyncFromLayout(layout);
		bottomDeck.SyncFromLayout(layout);
		RefreshFocusedViews();
		if (!layout.resources.empty()) {
			for (auto& chip : topBar.chips) {
				for (const auto& resource : layout.resources) {
					if (chip.id == resource.id) {
						chip.value = resource.value;
						chip.perSecond = resource.perSecond;
						chip.tint = resource.tint;
						chip.isCritical = resource.isCritical;
					}
				}
			}
		}
	}

	void SetMapMarkers(const std::vector<SpaceHUDMapMarker>& markers) {
		layout.mapMarkers = markers;
		RefreshFocusedViews();
	}

	void RefreshFocusedViews() {
		focusedQueue = BuildFocusedQueue();
		sideRail.SyncForFocus(activeResource, static_cast<int>(layout.selection.selectedUnits));
		galaxyPanel.SyncFromLayout(layout, activeResource);
		SetSelection(activeSelection);
	}

	bool HandleMousePress(float mx, float my, int button) {
		if (button != 1 && button != 3)
			return false;

		auto inRect = [&](const SpaceHUDRect& rect) {
			return (mx >= rect.x && mx <= rect.x + rect.w && my >= rect.y && my <= rect.y + rect.h);
		};

		auto inStripBand = [&](const SpaceHUDRect& rect, bool resourceBand) {
			const float splitY = rect.y + (rect.h * 0.60f);
			return resourceBand ? (my <= splitY) : (my > splitY);
		};

		auto chipIndexAt = [&](const std::vector<SpaceHUDResourceChip>& chips, const SpaceHUDRect& rect) -> int {
			if (chips.empty() || !inRect(rect) || !inStripBand(rect, true))
				return -1;

			const float step = rect.w / std::max<size_t>(1, chips.size());
			return std::clamp(static_cast<int>((mx - rect.x) / std::max(step, 0.0001f)), 0, static_cast<int>(chips.size() - 1));
		};

		if (const int resourceIdx = chipIndexAt(topBar.chips, layout.topStrip.bounds); resourceIdx >= 0) {
			SetActiveResource(topBar.chips[resourceIdx].id);
			if (button == 3) {
				showGalaxyOverlay = !showGalaxyOverlay;
				SetStatus(std::string("Pinned ") + topBar.chips[resourceIdx].label + " and toggled galaxy overlay");
			} else {
				SetStatus("Inspecting " + topBar.chips[resourceIdx].label);
			}
			return true;
		}

		if (inRect(topBar.panel.bounds) && inStripBand(topBar.panel.bounds, false)) {
			if (!topBar.buttons.empty()) {
				const float step = topBar.panel.bounds.w / std::max<size_t>(1, topBar.buttons.size());
				const size_t idx = std::min(topBar.buttons.size() - 1, static_cast<size_t>((mx - topBar.panel.bounds.x) / std::max(step, 0.0001f)));
				SetActiveTab(topBar.buttons[idx].id);
				SetStatus("Opened " + topBar.buttons[idx].label);
				return true;
			}
		}

		if (inRect(sideRail.panel.bounds)) {
			if (!sideRail.entries.empty()) {
				const float rowHeight = sideRail.panel.bounds.h / std::max<size_t>(1, sideRail.entries.size());
				const size_t idx = std::min(sideRail.entries.size() - 1, static_cast<size_t>((my - sideRail.panel.bounds.y) / std::max(rowHeight, 0.0001f)));
				SetSelection(sideRail.entries[idx].label);
				SetStatus("Selected " + sideRail.entries[idx].label);
				return true;
			}
		}

		if (inRect(bottomDeck.panel.bounds)) {
			if (!bottomDeck.tabs.empty()) {
				const float step = bottomDeck.panel.bounds.w / std::max<size_t>(1, bottomDeck.tabs.size());
				const size_t idx = std::min(bottomDeck.tabs.size() - 1, static_cast<size_t>((mx - bottomDeck.panel.bounds.x) / std::max(step, 0.0001f)));
				SetActiveTab(bottomDeck.tabs[idx].id);
				SetStatus("Switched to " + bottomDeck.tabs[idx].label + " commands");
				return true;
			}
		}

		if (inRect(galaxyPanel.panel.bounds)) {
			if (!galaxyPanel.visibleMarkers.empty()) {
				const float rowHeight = galaxyPanel.panel.bounds.h / std::max<size_t>(1, galaxyPanel.visibleMarkers.size());
				const size_t idx = std::min(galaxyPanel.visibleMarkers.size() - 1, static_cast<size_t>((my - galaxyPanel.panel.bounds.y) / std::max(rowHeight, 0.0001f)));
				const auto& marker = galaxyPanel.visibleMarkers[idx];
				SetSelection(marker.name);
				if (!marker.type.empty())
					SetActiveResource(marker.type == "power" ? "energy" : (marker.type == "storage" ? "storage" : "metal"));
				SetStatus("Focused marker " + marker.name);
				return true;
			}

			ToggleGalaxyOverlay(!showGalaxyOverlay);
			SetStatus(std::string("Galaxy overlay ") + (showGalaxyOverlay ? "enabled" : "disabled"));
			return true;
		}

		if (inRect(fleetPanel.panel.bounds)) {
			ToggleFleetPanels(!showFleetPanels);
			SetStatus(std::string("Fleet panel ") + (showFleetPanels ? "enabled" : "disabled"));
			return true;
		}

		if (inRect(researchPanel.panel.bounds)) {
			SetActiveTab("research");
			SetStatus("Research panel focused");
			return true;
		}

		return false;
	}

	void SetSelection(const std::string& selection) {
		activeSelection = selection;
		for (auto& entry : sideRail.entries) {
			entry.isSelected = (entry.label == selection || entry.id == selection);
		}
	}

	void SetActiveResource(const std::string& resourceId) {
		activeResource = resourceId;
		SetStatus("Economy focus: " + resourceId);
		RefreshFocusedViews();
	}

	std::vector<SpaceHUDQueueItem> BuildFocusedQueue() const {
		std::vector<SpaceHUDQueueItem> queue;
		const auto toLower = [](std::string value) {
			for (char& c : value) {
				c = char(std::tolower(static_cast<unsigned char>(c)));
			}
			return value;
		};

		std::vector<std::string> keywords;
		if (activeResource == "metal" || activeResource == "metal_income") {
			if (activeTab == "ships") {
				keywords = {"ship", "fighter", "bomber", "carrier", "frigate"};
			} else if (activeTab == "defense") {
				keywords = {"turret", "laser", "shield", "armor", "defense"};
			} else if (activeTab == "research") {
				keywords = {"research", "lab", "tech", "analysis"};
			} else if (activeTab == "structures") {
				keywords = {"storage", "factory", "mine", "yard", "warehouse"};
			} else {
				keywords = {"ship", "build", "factory", "mine", "recycler", "yard"};
			}
		} else if (activeResource == "energy" || activeResource == "energy_income") {
			if (activeTab == "research") {
				keywords = {"research", "lab", "tech", "analysis"};
			} else if (activeTab == "defense") {
				keywords = {"shield", "barrier", "turret", "pulse", "reactor"};
			} else if (activeTab == "ships") {
				keywords = {"energy", "power", "reactor", "thruster", "engine"};
			} else {
				keywords = {"energy", "power", "research", "shield", "reactor"};
			}
		} else {
			if (activeTab == "buildings") {
				keywords = {"storage", "logistics", "supply", "route", "warehouse"};
			} else if (activeTab == "research") {
				keywords = {"research", "science", "lab", "analysis"};
			} else {
				keywords = {"storage", "logistics", "supply", "route", "warehouse"};
			}
		}

		const auto addCard = [&](const SpaceHUDCommandCard& card) {
			const std::string label = toLower(card.name);
			bool matches = keywords.empty();
			for (const auto& keyword : keywords) {
				if (label.find(keyword) != std::string::npos) {
					matches = true;
					break;
				}
			}

			if (!matches)
				return;

			SpaceHUDQueueItem item;
			item.id = std::to_string(card.commandID);
			item.label = card.name;
			item.remainingSeconds = card.isBuildCommand ? 120 : 45;
			item.count = card.isDisabled ? 0 : 1;
			item.isActive = !card.isDisabled;
			queue.push_back(item);
		};

		if (!layout.commandCards.empty()) {
			for (const auto& card : layout.commandCards)
				addCard(card);
		} else {
			for (const auto& item : bottomDeck.queue) {
				const std::string label = toLower(item.label);
				bool matches = keywords.empty();
				for (const auto& keyword : keywords) {
					if (label.find(keyword) != std::string::npos) {
						matches = true;
						break;
					}
				}

				if (matches)
					queue.push_back(item);
			}
		}

		if (queue.empty())
			return bottomDeck.queue;

		return queue;
	}

	std::vector<std::string> BuildResourceFocusLines() const {
		std::vector<std::string> lines;
		lines.push_back("Resource focus: " + activeResource);

		const auto getChip = [&](const std::string& id) -> const SpaceHUDResourceChip* {
			for (const auto& chip : topBar.chips) {
				if (chip.id == id)
					return &chip;
			}
			return nullptr;
		};

		const SpaceHUDResourceChip* focused = getChip(activeResource);
		const SpaceHUDResourceChip* metal = getChip("metal");
		const SpaceHUDResourceChip* energy = getChip("energy");
		const SpaceHUDResourceChip* storage = getChip("storage");

		if (focused != nullptr) {
			lines.push_back(focused->label + " " + std::to_string(static_cast<int>(focused->value)) + " | " + std::to_string(focused->perSecond) + "/s");
		}

		if (activeResource == "metal" || activeResource == "metal_income") {
			lines.push_back("Metal economy is driving build capacity.");
			if (metal != nullptr)
				lines.push_back("Stockpile headroom: " + std::to_string(static_cast<int>(metal->value)));
		} else if (activeResource == "energy" || activeResource == "energy_income") {
			lines.push_back("Energy stability is the current bottleneck.");
			if (energy != nullptr)
				lines.push_back("Power reserve: " + std::to_string(static_cast<int>(energy->value)));
		} else {
			lines.push_back("Storage focus is showing combined logistics capacity.");
			if (storage != nullptr)
				lines.push_back("Combined storage: " + std::to_string(static_cast<int>(storage->value)));
		}

		lines.push_back(showGalaxyOverlay ? "Overlay: enabled" : "Overlay: disabled");
		return lines;
	}

	std::string GetTooltip(float mx, float my) const {
		auto inRect = [&](const SpaceHUDRect& rect) {
			return (mx >= rect.x && mx <= rect.x + rect.w && my >= rect.y && my <= rect.y + rect.h);
		};

		if (!topBar.chips.empty() && inRect(layout.topStrip.bounds)) {
			if (my > layout.topStrip.bounds.y + (layout.topStrip.bounds.h * 0.60f))
				return "Top bar controls\nUse the lower strip for menu tabs and the upper strip for economy chips.";

			const float step = layout.topStrip.bounds.w / std::max<size_t>(1, topBar.chips.size());
			const size_t idx = std::min(topBar.chips.size() - 1, static_cast<size_t>((mx - layout.topStrip.bounds.x) / std::max(step, 0.0001f)));
			const auto& chip = topBar.chips[idx];
			std::string tip = chip.label + "\nValue: " + std::to_string(static_cast<int>(chip.value));
			tip += "\nRate: " + std::to_string(chip.perSecond);
			tip += "\nLeft click to focus, right click to pin and toggle the galaxy overlay.";
			return tip;
		}

		if (inRect(topBar.panel.bounds))
			return "Top bar controls\nUse the lower strip for menu tabs and the upper strip for economy chips.";

		if (inRect(bottomDeck.panel.bounds))
			return "Construction deck\nSwitch build categories and review queue progress.";

		if (inRect(sideRail.panel.bounds))
			return "Empire rail\nSelect overview, messages, events, research, fleet, or galaxy.";

		if (inRect(researchPanel.panel.bounds))
			return "Research panel\nTrack active tech and progress.";

		if (inRect(fleetPanel.panel.bounds))
			return "Fleet panel\nReview ship counts and task groups.";

		if (inRect(galaxyPanel.panel.bounds))
			return "Galaxy view\nClick to focus the overlay.";

		return "";
	}

	void SetActiveTab(const std::string& tab) {
		activeTab = tab;
		for (auto& button : topBar.buttons) {
			button.isActive = (button.id == tab);
		}
		for (auto& entry : sideRail.entries) {
			entry.isSelected = (entry.id == tab);
		}
		for (auto& deckTab : bottomDeck.tabs) {
			deckTab.isActive = (deckTab.id == tab);
		}
		RefreshFocusedViews();
	}

	void SetStatus(const std::string& status) {
		activeStatus = status;
	}

	void ToggleGalaxyOverlay(bool enabled) {
		showGalaxyOverlay = enabled;
	}

	void ToggleBuildQueue(bool enabled) {
		showBuildQueue = enabled;
	}

	void ToggleFleetPanels(bool enabled) {
		showFleetPanels = enabled;
	}
};

} // namespace arclight
