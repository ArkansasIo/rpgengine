/* ArcLight Engine - Game Settings & Options
 * Developer: Stephen
 * Centralized settings management for graphics, audio, input, and gameplay.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <vector>

namespace arclight {

using SettingValue = std::variant<int, float, bool, std::string>;

struct SettingDef {
	std::string key;
	std::string category;
	std::string displayName;
	std::string description;
	SettingValue defaultValue;
	SettingValue minValue;
	SettingValue maxValue;
};

// ======================== Graphics Settings ========================
struct GraphicsSettings {
	bool fullscreen = false;
	bool vsync = true;
	int resolutionX = 1920;
	int resolutionY = 1080;
	int displayMode = 0; // 0=windowed, 1=borderless, 2=fullscreen
	float renderScale = 1.0f;

	// Quality
	int shadowQuality = 2; // 0=off, 1=low, 2=medium, 3=high, 4=ultra
	int textureQuality = 2;
	int antiAliasing = 2; // 0=off, 1=FXAA, 2=MSAA2x, 3=MSAA4x
	int postProcessing = 2;
	int particleQuality = 2;
	int terrainQuality = 2;
	int waterQuality = 2;
	int skyQuality = 2;

	// Advanced
	bool hdr = false;
	bool motionBlur = false;
	float motionBlurAmount = 0.5f;
	bool bloom = true;
	float bloomIntensity = 0.3f;
	bool ambientOcclusion = true;
	int aoQuality = 1; // 0=off, 1=SSAO, 2=GTAO
	bool screenSpaceReflections = true;
	float drawDistance = 5000.0f;
	float foliageDrawDistance = 3000.0f;
	bool vsyncAdaptive = false;
	int maxFPS = 0; // 0 = uncapped
	float gamma = 1.0f;
	float brightness = 1.0f;
	float contrast = 1.0f;

	void ApplyPreset(int preset) {
		switch (preset) {
			case 0: // Low
				shadowQuality = 0; textureQuality = 0; antiAliasing = 0;
				postProcessing = 0; particleQuality = 0; terrainQuality = 0;
				waterQuality = 0; skyQuality = 0; bloom = false;
				ambientOcclusion = false; screenSpaceReflections = false;
				drawDistance = 2000; foliageDrawDistance = 1000;
				break;
			case 1: // Medium
				shadowQuality = 1; textureQuality = 1; antiAliasing = 1;
				postProcessing = 1; particleQuality = 1; terrainQuality = 1;
				waterQuality = 1; skyQuality = 1; bloom = true;
				ambientOcclusion = false; screenSpaceReflections = false;
				drawDistance = 3000; foliageDrawDistance = 2000;
				break;
			case 2: // High
				shadowQuality = 2; textureQuality = 2; antiAliasing = 2;
				postProcessing = 2; particleQuality = 2; terrainQuality = 2;
				waterQuality = 2; skyQuality = 2; bloom = true;
				ambientOcclusion = true; screenSpaceReflections = true;
				drawDistance = 5000; foliageDrawDistance = 3000;
				break;
			case 3: // Ultra
				shadowQuality = 4; textureQuality = 4; antiAliasing = 3;
				postProcessing = 3; particleQuality = 3; terrainQuality = 3;
				waterQuality = 3; skyQuality = 3; bloom = true;
				ambientOcclusion = true; screenSpaceReflections = true;
				drawDistance = 8000; foliageDrawDistance = 5000;
				break;
		}
	}
};

// ======================== Audio Settings ========================
struct AudioSettings {
	float masterVolume = 1.0f;
	float musicVolume = 0.8f;
	float sfxVolume = 1.0f;
	float voiceVolume = 1.0f;
	float ambientVolume = 0.6f;
	float uiVolume = 0.8f;
	bool muteAll = false;
	bool muteOnFocusLoss = true;
	int audioDevice = 0;
	int outputMode = 0; // 0=stereo, 1=surround, 2=headphone
	float reverbAmount = 0.5f;
	float dopplerFactor = 1.0f;
	int maxSimultaneousSounds = 64;
	std::string language = "English";
};

// ======================== Input Settings ========================
struct InputSettings {
	float mouseSensitivity = 1.0f;
	float mouseSensitivityX = 1.0f;
	float mouseSensitivityY = 1.0f;
	bool invertMouseY = false;
	bool invertMouseX = false;
	float scrollSpeed = 1.0f;
	float cameraRotationSpeed = 180.0f;
	float cameraPanSpeed = 500.0f;
	float cameraZoomSpeed = 5.0f;
	bool edgeScrolling = true;
	int edgeScrollMargin = 20;
	float edgeScrollSpeed = 400.0f;
	bool doubleClickSelect = true;
	bool holdToDrag = false;
	float doubleClickTime = 0.3f;
	int keyRepeatDelay = 500;
	int keyRepeatRate = 30;
	bool rawMouseInput = false;
	float gamepadDeadzone = 0.15f;
	float gamepadSensitivity = 1.0f;
	bool gamepadVibration = true;
};

// ======================== Gameplay Settings ========================
struct GameplaySettings {
	float gameSpeed = 1.0f;
	bool autoSave = true;
	int autoSaveInterval = 300;
	bool showTutorial = true;
	bool showHints = true;
	bool confirmOrders = false;
	bool enableAllyEnemyHighlight = true;
	bool showHealthBars = true;
	bool showDamageNumbers = false;
	int minimapSize = 1; // 0=small, 1=medium, 2=large
	bool minimapExpandOnHover = true;
	bool showFPS = false;
	bool showUnitNames = false;
	float cameraFOV = 60.0f;
	float cameraNearPlane = 1.0f;
	float cameraFarPlane = 10000.0f;
	bool cameraShake = true;
	float cameraShakeIntensity = 1.0f;
	bool autoFormation = true;
	int defaultFormation = 0;
};

// ======================== Network Settings ========================
struct NetworkSettings {
	std::string playerName = "Player";
	int maxPing = 300;
	bool showPing = true;
	bool autoReconnect = true;
	int reconnectAttempts = 3;
	bool useCompression = true;
	int connectionTimeout = 30;
	bool NATPunchthrough = false;
	std::string proxyAddress = "";
	int proxyPort = 0;
};

// ======================== Master Settings Manager ========================
class GameSettings {
public:
	GraphicsSettings graphics;
	AudioSettings audio;
	InputSettings input;
	GameplaySettings gameplay;
	NetworkSettings network;

	void Init() {
		ApplyDefaults();
	}

	void ApplyDefaults() {
		graphics = GraphicsSettings();
		audio = AudioSettings();
		input = InputSettings();
		gameplay = GameplaySettings();
		network = NetworkSettings();
	}

	void SetGraphicsPreset(int preset) {
		graphics.ApplyPreset(preset);
		if (onGraphicsChanged) onGraphicsChanged();
	}

	void SaveToFile(const std::string& path) {
		if (onSave) onSave(path, *this);
	}

	void LoadFromFile(const std::string& path) {
		if (onLoad) onLoad(path, *this);
	}

	std::function<void()> onGraphicsChanged;
	std::function<void()> onAudioChanged;
	std::function<void()> onInputChanged;
	std::function<void()> onGameplayChanged;
	std::function<void(const std::string&, const GameSettings&)> onSave;
	std::function<void(const std::string&, GameSettings&)> onLoad;
};

} // namespace arclight
