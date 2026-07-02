/* ArcLight Engine - Display Settings
 * Developer: Stephen
 * Resolution, monitor, display mode, and window management.
 */

#pragma once

#include <string>
#include <vector>

namespace arclight {

struct DisplayMode {
	int width = 1920;
	int height = 1080;
	int refreshRate = 60;
};

struct MonitorInfo {
	std::string name;
	int index = 0;
	int x = 0, y = 0;
	int width = 1920, int height = 1080;
	bool isPrimary = false;
	float dpiScale = 1.0f;
};

class DisplaySettings {
public:
	void Init() {
		// Detect available display modes
		availableModes = {
			{1280, 720, 60}, {1280, 720, 120}, {1280, 720, 144},
			{1366, 768, 60},
			{1600, 900, 60},
			{1920, 1080, 60}, {1920, 1080, 120}, {1920, 1080, 144}, {1920, 1080, 240},
			{2560, 1440, 60}, {2560, 1440, 120}, {2560, 1440, 144},
			{3840, 2160, 60}, {3840, 2160, 120},
		};
	}

	void SetResolution(int width, int height, int refreshRate = 60) {
		resolution.width = width;
		resolution.height = height;
		resolution.refreshRate = refreshRate;
		if (onResolutionChanged) onResolutionChanged(width, height);
	}

	void SetDisplayMode(int mode) {
		displayMode = mode; // 0=windowed, 1=borderless, 2=exclusive
		if (onDisplayModeChanged) onDisplayModeChanged(mode);
	}

	void SetMonitor(int index) {
		primaryMonitor = index;
	}

	void SetVSync(bool enabled) {
		vsync = enabled;
	}

	void SetFullscreen(bool fullscreen) {
		if (fullscreen) displayMode = 2;
		else displayMode = 0;
	}

	const DisplayMode& GetResolution() const { return resolution; }
	int GetDisplayMode() const { return displayMode; }
	bool IsVSyncEnabled() const { return vsync; }
	bool IsFullscreen() const { return displayMode == 2; }

	std::vector<DisplayMode> GetAvailableModes() const { return availableModes; }

	std::function<void(int, int)> onResolutionChanged;
	std::function<void(int)> onDisplayModeChanged;

private:
	DisplayMode resolution = {1920, 1080, 60};
	int displayMode = 0;
	bool vsync = true;
	int primaryMonitor = 0;
	std::vector<DisplayMode> availableModes;
};

} // namespace arclight
