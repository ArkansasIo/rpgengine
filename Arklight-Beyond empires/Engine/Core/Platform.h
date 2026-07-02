/* OGame Beyond - Platform Abstraction Layer
 * Developer: Stephen
 * Cross-platform support for PC, PS5, Xbox Series X|S, Nintendo Switch.
 * Provides unified APIs for graphics, input, audio, networking, and storage
 * across all target platforms.
 */
#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace ogb {

// ======================== Platform Detection ========================

enum class EPlatform {
    Windows,
    Linux,
    macOS,
    PS5,
    XboxSeriesX,
    XboxSeriesS,
    NintendoSwitch,
    iOS,
    Android,
    WebAssembly,
};

enum class EGraphicsAPI {
    Vulkan,
    DirectX12,
    Metal,
    OpenGL45,
    OpenGL33,
    GNM,        // PS5 native
    GDK,        // Xbox Game Development Kit
    NullGraphics,
};

enum class EAudioAPI {
    OpenALSoft,
    FMOD,
    Audio3D,    // PS5 3D Audio
    XAudio2,    // Xbox
    CoreAudio,  // macOS/iOS
    NullAudio,
};

enum class EInputAPI {
    SDL3,
    DualSense,  // PS5
    GDK,        // Xbox
    JoyCon,     // Switch
    NullInput,
};

enum class ENetworkingAPI {
    ENet,
    Sockets,
    SN-Sessions, // PS5
    GDKNetworking, // Xbox
    NullNetworking,
};

struct PlatformConfig {
    EPlatform platform = EPlatform::Windows;
    EGraphicsAPI graphicsAPI = EGraphicsAPI::Vulkan;
    EAudioAPI audioAPI = EAudioAPI::OpenALSoft;
    EInputAPI inputAPI = EInputAPI::SDL3;
    ENetworkingAPI networkAPI = ENetworkingAPI::ENet;
    std::string platformName = "PC";
    int maxMemoryMB = 16384;
    int cpuCores = 8;
    bool hasGPU = true;
    bool hasRayTracing = false;
    bool hasHapticFeedback = false;
    bool hasTouchScreen = false;
    int maxControllers = 4;
};

// ======================== Platform Info ========================

struct PlatformInfo {
    std::string osName;
    std::string osVersion;
    std::string cpuName;
    int cpuCores = 0;
    int64_t totalMemoryMB = 0;
    std::string gpuName;
    std::string gpuDriver;
    int gpuVRAMMB = 0;
    bool hasRayTracing = false;
    bool hasMeshShaders = false;
    int maxTextureSize = 16384;
};

// ======================== Platform Abstraction ========================

class PlatformLayer {
public:
    PlatformConfig config;
    PlatformInfo info;

    void Init(PlatformConfig cfg) {
        config = cfg;
        DetectPlatform();
        DetectHardware();
    }

    EPlatform GetPlatform() const { return config.platform; }
    const std::string& GetPlatformName() const { return config.platformName; }
    bool IsConsole() const {
        return config.platform == EPlatform::PS5 ||
               config.platform == EPlatform::XboxSeriesX ||
               config.platform == EPlatform::XboxSeriesS ||
               config.platform == EPlatform::NintendoSwitch;
    }
    bool IsPC() const {
        return config.platform == EPlatform::Windows ||
               config.platform == EPlatform::Linux ||
               config.platform == EPlatform::macOS;
    }
    bool IsMobile() const {
        return config.platform == EPlatform::iOS ||
               config.platform == EPlatform::Android;
    }

    // Console-specific features
    bool SupportsHapticFeedback() const { return config.hasHapticFeedback; }
    bool SupportsRayTracing() const { return config.hasRayTracing; }
    int GetMaxControllers() const { return config.maxControllers; }

    // Platform callbacks
    std::function<void()> onResume;
    std::function<void()> onPause;
    std::function<void()> onLowMemory;
    std::function<void()> onControllerConnected;
    std::function<void()> onControllerDisconnected;

private:
    void DetectPlatform() {
#ifdef _WIN32
        config.platform = EPlatform::Windows;
        config.platformName = "Windows";
#elif __linux__
        config.platform = EPlatform::Linux;
        config.platformName = "Linux";
#elif __APPLE__
        config.platform = EPlatform::macOS;
        config.platformName = "macOS";
#elif __ps5__
        config.platform = EPlatform::PS5;
        config.platformName = "PlayStation 5";
        config.graphicsAPI = EGraphicsAPI::GNM;
        config.audioAPI = EAudioAPI::Audio3D;
        config.inputAPI = EInputAPI::DualSense;
        config.networkAPI = ENetworkingAPI::SN_Sessions;
        config.hasHapticFeedback = true;
        config.hasRayTracing = true;
        config.maxMemoryMB = 16384;
        config.cpuCores = 8;
#elif __XBOX_SERIES_X__
        config.platform = EPlatform::XboxSeriesX;
        config.platformName = "Xbox Series X";
        config.graphicsAPI = EGraphicsAPI::DirectX12;
        config.audioAPI = EAudioAPI::XAudio2;
        config.inputAPI = EInputAPI::GDK;
        config.networkAPI = ENetworkingAPI::GDKNetworking;
        config.hasRayTracing = true;
        config.maxMemoryMB = 16384;
        config.cpuCores = 8;
#elif __XBOX_SERIES_S__
        config.platform = EPlatform::XboxSeriesS;
        config.platformName = "Xbox Series S";
        config.graphicsAPI = EGraphicsAPI::DirectX12;
        config.audioAPI = EAudioAPI::XAudio2;
        config.inputAPI = EInputAPI::GDK;
        config.networkAPI = ENetworkingAPI::GDKNetworking;
        config.maxMemoryMB = 8192;
        config.cpuCores = 8;
#elif __SWITCH__
        config.platform = EPlatform::NintendoSwitch;
        config.platformName = "Nintendo Switch";
        config.graphicsAPI = EGraphicsAPI::NVN; // custom
        config.inputAPI = EInputAPI::JoyCon;
        config.hasTouchScreen = true;
        config.maxMemoryMB = 4096;
        config.cpuCores = 4;
#elif __EMSCRIPTEN__
        config.platform = EPlatform::WebAssembly;
        config.platformName = "WebAssembly";
        config.graphicsAPI = EGraphicsAPI::WebGL2;
        config.maxMemoryMB = 2048;
        config.cpuCores = 4;
#endif
    }

    void DetectHardware() {
        info.osName = config.platformName;
        info.cpuCores = config.cpuCores;
        info.totalMemoryMB = config.maxMemoryMB;
        info.hasRayTracing = config.hasRayTracing;
    }
};

// ======================== Platform-Specific SDK Interfaces ========================

// PS5 SDK
struct PS5SDK {
    void Init() {}
    void Shutdown() {}
    void EnableHaptic(bool enabled) {}
    void SetHapticFeedback(int controller, float intensity, float frequency) {}
    void TriggerAdaptiveTrigger(int controller, int trigger, float position, float strength) {}
    void Enable3DAudio(const float3& listenerPos, const float3& listenerForward) {}
    void SetActivity(const std::string& name) {}
    void EnableSSO(const std::string& npTitleId) {}
    void ShowTrophyNotification(const std::string& trophyID) {}
    void SetActivityData(const std::string& data) {}
};

// Xbox GDK
struct XboxGDK {
    void Init() {}
    void Shutdown() {}
    void EnableRichPresence(const std::string&RichStringId, const std::string& args = "") {}
    void DisableRichPresence() {}
    void ShowGameSaveUI() {}
    void EnableSecureRoam() {}
    void SetGamePadRumble(int controller, float leftMotor, float rightMotor) {}
    void ReportAchievement(const std::string& achievementID, int percentComplete) {}
    void SetUserColor(int r, int g, int b) {}
    void ShowInviteUI() {}
    void ShowMarketplace() {}
};

// Nintendo Switch
struct SwitchSDK {
    void Init() {}
    void Shutdown() {}
    void EnableVibration(int controller, float lowAmplitude, float highAmplitude) {}
    void SetNFCData(const std::string& data) {}
    void EnableIRSensor(int controller, bool enabled) {}
    void SetCPUBoost(bool enabled) {}
    void ShowSoftwareKeyboard(const std::string& title, int maxChars, std::function<void(const std::string&)> callback) {}
};

// ======================== Cross-Platform Input ========================

enum class EGamepadButton {
    A, B, X, Y,
    LeftBumper, RightBumper,
    LeftTrigger, RightTrigger,
    LeftStick, RightStick,
    DPadUp, DPadDown, DPadLeft, DPadRight,
    Start, Back, Touchpad,
    Share, // PS5
    View, Menu, // Xbox
    Capture, // Switch
};

enum class EGamepadAxis {
    LeftX, LeftY,
    RightX, RightY,
    LeftTrigger, RightTrigger,
};

struct GamepadState {
    bool connected = false;
    int playerIndex = 0;
    float axes[6] = {};
    bool buttons[16] = {};
    bool prevButtons[16] = {};
    float leftTrigger = 0, rightTrigger = 0;

    bool IsButtonPressed(EGamepadButton btn) const {
        int idx = static_cast<int>(btn);
        return idx < 16 && buttons[idx];
    }

    bool IsButtonJustPressed(EGamepadButton btn) const {
        int idx = static_cast<int>(btn);
        return idx < 16 && buttons[idx] && !prevButtons[idx];
    }

    float GetAxis(EGamepadAxis axis) const {
        int idx = static_cast<int>(axis);
        return idx < 6 ? axes[idx] : 0;
    }

    void SetHaptic(float leftMotor, float rightMotor, float duration = 0.5f) {
        // Platform-specific haptic call
    }

    void SetAdaptiveTrigger(int trigger, float position, float strength) {
        // PS5 DualSense adaptive triggers
    }
};

// ======================== Platform Save System ========================

struct PlatformSaveData {
    std::string slotName;
    std::vector<uint8_t> data;
    std::string timestamp;
    int version = 1;
};

class PlatformSaveSystem {
public:
    bool Save(const std::string& slot, const std::vector<uint8_t>& data) {
        PlatformSaveData save;
        save.slotName = slot;
        save.data = data;
        saves[slot] = save;
        return true;
    }

    bool Load(const std::string& slot, std::vector<uint8_t>& data) {
        auto it = saves.find(slot);
        if (it != saves.end()) { data = it->second.data; return true; }
        return false;
    }

    void DeleteSlot(const std::string& slot) { saves.erase(slot); }

    bool SlotExists(const std::string& slot) const { return saves.count(slot); }

    void SetCloudSaveEnabled(bool enabled) { cloudEnabled = enabled; }
    bool IsCloudSaveEnabled() const { return cloudEnabled; }

private:
    std::unordered_map<std::string, PlatformSaveData> saves;
    bool cloudEnabled = false;
};

// ======================== Trophy / Achievement System ========================

struct TrophyDef {
    std::string id;
    std::string name;
    std::string description;
    std::string iconPath;
    bool isHidden = false;
    float progressRequired = 1.0f;
};

class PlatformTrophySystem {
public:
    void Init() {}
    void RegisterTrophy(const TrophyDef& def) { trophies[def.id] = def; }

    void UnlockTrophy(const std::string& id) {
        if (trophies.count(id)) {
            unlocked.insert(id);
            if (onTrophyUnlocked) onTrophyUnlocked(id);
        }
    }

    void UpdateProgress(const std::string& id, float progress) {
        if (trophies.count(id) && progress >= trophies[id].progressRequired) {
            UnlockTrophy(id);
        }
    }

    bool IsUnlocked(const std::string& id) const { return unlocked.count(id); }
    float GetProgress(const std::string& id) const { return 0; }
    int GetUnlockedCount() const { return static_cast<int>(unlocked.size()); }
    int GetTotalCount() const { return static_cast<int>(trophies.size()); }

    std::function<void(const std::string&)> onTrophyUnlocked;

private:
    std::unordered_map<std::string, TrophyDef> trophies;
    std::unordered_set<std::string> unlocked;
};

// ======================== Cross-Platform Gamepad ========================

class GamepadManager {
public:
    std::vector<GamepadState> gamepads;

    void Init(int maxControllers = 4) {
        gamepads.resize(maxControllers);
    }

    void Update(float dt) {
        for (auto& gp : gamepads) {
            gp.prevButtons[0] = gp.buttons[0];
            gp.prevButtons[1] = gp.buttons[1];
            // etc
        }
    }

    GamepadState& GetGamepad(int index) {
        if (index < 0 || index >= static_cast<int>(gamepads.size())) {
            static GamepadState empty;
            return empty;
        }
        return gamepads[index];
    }

    void SetButton(int controller, EGamepadButton btn, bool pressed) {
        if (controller >= 0 && controller < static_cast<int>(gamepads.size())) {
            gamepads[controller].buttons[static_cast<int>(btn)] = pressed;
        }
    }

    void SetAxis(int controller, EGamepadAxis axis, float value) {
        if (controller >= 0 && controller < static_cast<int>(gamepads.size())) {
            gamepads[controller].axes[static_cast<int>(axis)] = value;
        }
    }

    void SetVibration(int controller, float left, float right, float duration = 0.5f) {
        if (controller >= 0 && controller < static_cast<int>(gamepads.size())) {
            gamepads[controller].SetHaptic(left, right, duration);
        }
    }

    void SetAdaptiveTrigger(int controller, int trigger, float position, float strength) {
        if (controller >= 0 && controller < static_cast<int>(gamepads.size())) {
            gamepads[controller].SetAdaptiveTrigger(trigger, position, strength);
        }
    }

    int GetConnectedCount() const {
        int count = 0;
        for (auto& gp : gamepads) if (gp.connected) count++;
        return count;
    }
};

} // namespace ogb
