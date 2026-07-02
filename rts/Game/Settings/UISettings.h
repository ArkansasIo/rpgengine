/* ArcLight Engine - UI Settings
 * Developer: Stephen
 * HUD, tooltip, and interface customization.
 */

#pragma once

#include <string>

namespace arclight {

struct UIScale {
	float hudScale = 1.0f;
	float textScale = 1.0f;
	float iconScale = 1.0f;
	float tooltipScale = 1.0f;
};

struct UIColors {
	float hudColor[4] = {0.2f, 0.6f, 1.0f, 0.8f};
	float healthBarGreen[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	float healthBarRed[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	float shieldBarBlue[4] = {0.2f, 0.4f, 1.0f, 0.8f};
	float selectionRingColor[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	float enemyColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	float allyColor[4] = {0.0f, 0.8f, 0.0f, 1.0f};
	float neutralColor[4] = {1.0f, 1.0f, 0.0f, 1.0f};
	float buildingColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	float resourceColor[4] = {1.0f, 0.8f, 0.2f, 1.0f};
	float damageNumberColor[4] = {1.0f, 0.2f, 0.2f, 1.0f};
	float healNumberColor[4] = {0.2f, 1.0f, 0.2f, 1.0f};
};

struct HUDLayout {
	// Minimap
	float minimapX = 0.0f;
	float minimapY = -10.0f; // anchored bottom-right
	float minimapWidth = 250.0f;
	float minimapHeight = 250.0f;
	bool minimapShowTerrain = true;
	bool minimapShowUnits = true;
	bool minimapShowBuildings = true;
	bool minimapShowResources = true;
	int minimapZoomLevel = 1;

	// Resource bar
	float resourceBarX = 0.3f;
	float resourceBarY = 0.0f;
	float resourceBarWidth = 0.4f;
	float resourceBarHeight = 30.0f;

	// Unit info panel
	float infoPanelX = 0.0f;
	float infoPanelY = -50.0f;
	float infoPanelWidth = 300.0f;
	float infoPanelHeight = 120.0f;

	// Command panel
	float commandPanelX = -10.0f;
	float commandPanelY = -10.0f;
	float commandPanelWidth = 250.0f;
	float commandPanelHeight = 200.0f;

	// Chat
	float chatX = 0.0f;
	float chatY = 10.0f;
	float chatWidth = 400.0f;
	float chatHeight = 200.0f;
	int chatMaxLines = 50;

	// FPS counter
	float fpsX = 10.0f;
	float fpsY = 10.0f;
	bool showFPS = false;
	bool showPing = true;
	bool showUnitCount = true;

	// Production queue
	float queueX = -10.0f;
	float queueY = -250.0f;
	float queueWidth = 200.0f;
	float queueHeight = 100.0f;
};

class UISettings {
public:
	UIScale scale;
	UIColors colors;
	HUDLayout layout;

	void SetHUDScale(float s) {
		scale.hudScale = s;
		if (onUIScaleChanged) onUIScaleChanged();
	}

	void SetTextScale(float s) {
		scale.textScale = s;
		if (onUIScaleChanged) onUIScaleChanged();
	}

	void ToggleMinimap() {
		layout.minimapShowTerrain = !layout.minimapShowTerrain;
		layout.minimapShowUnits = !layout.minimapShowUnits;
	}

	void SetMinimapSize(int size) {
		switch (size) {
			case 0: layout.minimapWidth = 180; layout.minimapHeight = 180; break;
			case 1: layout.minimapWidth = 250; layout.minimapHeight = 250; break;
			case 2: layout.minimapWidth = 350; layout.minimapHeight = 350; break;
		}
	}

	std::function<void()> onUIScaleChanged;
	std::function<void()> onHUDLayoutChanged;
};

} // namespace arclight
