/* OGame Beyond - Platform Build Configuration
 * Developer: Stephen
 * Per-platform build settings, SDK paths, and feature flags.
 */
#pragma once

// ======================== Platform Macros ========================

#if defined(_WIN32)
    #define OGB_PLATFORM_WINDOWS 1
    #define OGB_PLATFORM_NAME "Windows"
    #define OGB_PLATFORM_FAMILY "Desktop"
    #define OGB_EXPORT __declspec(dllexport)
    #define OGB_IMPORT __declspec(dllimport)
    #define OGB_API __cdecl
    #define OGB_PATH_SEP "\\"
#elif defined(__linux__)
    #define OGB_PLATFORM_LINUX 1
    #define OGB_PLATFORM_NAME "Linux"
    #define OGB_PLATFORM_FAMILY "Desktop"
    #define OGB_EXPORT __attribute__((visibility("default")))
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define OGB_PLATFORM_IOS 1
        #define OGB_PLATFORM_NAME "iOS"
        #define OGB_PLATFORM_FAMILY "Mobile"
    #else
        #define OGB_PLATFORM_MACOS 1
        #define OGB_PLATFORM_NAME "macOS"
        #define OGB_PLATFORM_FAMILY "Desktop"
    #endif
    #define OGB_EXPORT __attribute__((visibility("default")))
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#elif defined(__ps5__)
    #define OGB_PLATFORM_PS5 1
    #define OGB_PLATFORM_NAME "PlayStation 5"
    #define OGB_PLATFORM_FAMILY "Console"
    #define OGB_EXPORT
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#elif defined(__XBOX_SERIES_X__)
    #define OGB_PLATFORM_XBOX_SERIES_X 1
    #define OGB_PLATFORM_NAME "Xbox Series X"
    #define OGB_PLATFORM_FAMILY "Console"
    #define OGB_EXPORT
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#elif defined(__XBOX_SERIES_S__)
    #define OGB_PLATFORM_XBOX_SERIES_S 1
    #define OGB_PLATFORM_NAME "Xbox Series S"
    #define OGB_PLATFORM_FAMILY "Console"
    #define OGB_EXPORT
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#elif defined(__SWITCH__)
    #define OGB_PLATFORM_SWITCH 1
    #define OGB_PLATFORM_NAME "Nintendo Switch"
    #define OGB_PLATFORM_FAMILY "Console"
    #define OGB_EXPORT
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#elif defined(__EMSCRIPTEN__)
    #define OGB_PLATFORM_WASM 1
    #define OGB_PLATFORM_NAME "WebAssembly"
    #define OGB_PLATFORM_FAMILY "Web"
    #define OGB_EXPORT
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#else
    #define OGB_PLATFORM_UNKNOWN 1
    #define OGB_PLATFORM_NAME "Unknown"
    #define OGB_PLATFORM_FAMILY "Unknown"
    #define OGB_EXPORT
    #define OGB_IMPORT
    #define OGB_API
    #define OGB_PATH_SEP "/"
#endif

// ======================== Console SDK Selection ========================

#if defined(OGB_PLATFORM_PS5)
    #define OGB_USE_PSVRM 1      // PS5 VR support
    #define OGB_USE_DUALSENSE 1  // DualSense haptics
    #define OGB_USE_3DAUDIO 1    // PS5 Tempest 3D Audio
    #define OGB_USE_ACTIVITY 1   // PS5 Activities
    #define OGB_USE_TROPHIES 1   // PS5 Trophies
    #define OGB_USE_SSO 1        // PlayStation Network SSO
    #define OGB_USE_REST_MODE 1  // PS5 Rest Mode support
    #define OGB_USE_HDMI_REC 1   // HDMI Recorder
    #define OGB_MAX_TEXTURE_SIZE 16384
    #define OGB_HAS_RAY_TRACING 1
    #define OGB_HAS_MESH_SHADERS 1
    #define OGB_MAX_MEMORY_MB 16384
    #define OGB_CPU_CORES 8
    #define OGB_GPU_NAME "AMD RDNA 2 (10.28 TFLOPS)"
#elif defined(OGB_PLATFORM_XBOX_SERIES_X)
    #define OGB_USE_GDK 1         // Xbox GDK
    #define OGB_USE_RICH_PRESENCE 1
    #define OGB_USE_SMART_DELIVERY 1
    #define OGB_USE_GAME_PASS 1
    #define OGB_USE_CROSSPLAY 1
    #define OGB_USE_SECURE_ROAM 1
    #define OGB_USE_RAY_TRACING 1
    #define OGB_MAX_TEXTURE_SIZE 16384
    #define OGB_MAX_MEMORY_MB 16384
    #define OGB_CPU_CORES 8
    #define OGB_GPU_NAME "AMD RDNA 2 (12 TFLOPS)"
#elif defined(OGB_PLATFORM_XBOX_SERIES_S)
    #define OGB_USE_GDK 1
    #define OGB_USE_SMART_DELIVERY 1
    #define OGB_USE_GAME_PASS 1
    #define OGB_USE_CROSSPLAY 1
    #define OGB_MAX_TEXTURE_SIZE 8192
    #define OGB_MAX_MEMORY_MB 8192
    #define OGB_CPU_CORES 8
    #define OGB_GPU_NAME "AMD RDNA 2 (4 TFLOPS)"
#elif defined(OGB_PLATFORM_SWITCH)
    #define OGB_USE_NVN 1        // Nintendo NVN graphics API
    #define OGB_USE_NFP 1        // NFC/Amiibo support
    #define OGB_USE_IR 1         // IR sensor
    #define OGB_USE_HDCP 1       // HDCP content protection
    #define OGB_MAX_TEXTURE_SIZE 4096
    #define OGB_MAX_MEMORY_MB 4096
    #define OGB_CPU_CORES 4
    #define OGB_GPU_NAME "NVIDIA Tegra X1+ (1 TFLOP)"
    #define OGB_DOCKED_MODE 1
#elif defined(OGB_PLATFORM_WINDOWS)
    #define OGB_USE_DX12 1
    #define OGB_USE_VULKAN 1
    #define OGB_USE_DIRECTINPUT 1
    #define OGB_USE_XINPUT 1
    #define OGB_USE_WINRT 1
    #define OGB_USE_WGF 1       // Windows Gaming Framework
    #define OGB_USE_WGI 1       // Windows.Gaming.Input
    #define OGB_MAX_TEXTURE_SIZE 16384
    #define OGB_HAS_RAY_TRACING 1
    #define OGB_HAS_MESH_SHADERS 1
    #define OGB_MAX_MEMORY_MB 32768
    #define OGB_CPU_CORES 16
    #define OGB_GPU_NAME "Detected at runtime"
#elif defined(OGB_PLATFORM_LINUX)
    #define OGB_USE_VULKAN 1
    #define OGB_USE_WAYLAND 1
    #define OGB_USE_X11 1
    #define OGB_MAX_TEXTURE_SIZE 16384
    #define OGB_HAS_RAY_TRACING 1
    #define OGB_MAX_MEMORY_MB 32768
    #define OGB_CPU_CORES 16
#elif defined(OGB_PLATFORM_MACOS)
    #define OGB_USE_METAL 1
    #define OGB_USE_VULKAN_MOLTENVK 1
    #define OGB_USE_GAMECONTROLLER 1
    #define OGB_MAX_TEXTURE_SIZE 16384
    #define OGB_MAX_MEMORY_MB 32768
    #define OGB_CPU_CORES 12
#elif defined(OGB_PLATFORM_IOS)
    #define OGB_USE_METAL 1
    #define OGB_USE_GAMECONTROLLER 1
    #define OGB_USE_TOUCHSCREEN 1
    #define OGB_MAX_TEXTURE_SIZE 8192
    #define OGB_MAX_MEMORY_MB 6144
    #define OGB_CPU_CORES 6
#elif defined(OGB_PLATFORM_WASM)
    #define OGB_USE_WEBGL2 1
    #define OGB_USE_WEBAUDIO 1
    #define OGB_USE_WASM_THREADS 1
    #define OGB_MAX_TEXTURE_SIZE 4096
    #define OGB_MAX_MEMORY_MB 2048
    #define OGB_CPU_CORES 4
#endif

// ======================== Shared Console Features ========================

#if defined(OGB_PLATFORM_PS5) || defined(OGB_PLATFORM_XBOX_SERIES_X) || defined(OGB_PLATFORM_XBOX_SERIES_S)
    #define OGB_IS_CONSOLE 1
    #define OGB_CONSOLE_CERTIFICATION 1
    #define OGB_CONSOLE_SUSPEND 1
    #define OGB_CONSOLE_RATINGS 1
    #define OGB_CONSOLE_SOCIAL 1
    #define OGB_CONSOLE_VOICE_CHAT 1
    #define OGB_CONSOLE_SCREENSHOT 1
    #define OGB_CONSOLE_VIDEO_CAPTURE 1
    #define OGB_CONSOLE_ACTIVITY_FEED 1
    #define OGB_CONSOLE_INSTALL_SIZE_LIMIT 1
    #define OGB_CONSOLE_FRAME_RATE_TARGET 60
#elif defined(OGB_PLATFORM_SWITCH)
    #define OGB_IS_CONSOLE 1
    #define OGB_CONSOLE_SUSPEND 1
    #define OGB_CONSOLE_RATINGS 1
    #define OGB_CONSOLE_SOCIAL 1
    #define OGB_CONSOLE_SCREENSHOT 1
    #define OGB_CONSOLE_FRAME_RATE_TARGET 30
    #define OGB_CONSOLE_HANDHELD_MODE 1
    #define OGB_CONSOLE_DOCKED_MODE 1
#else
    #define OGB_IS_CONSOLE 0
#endif

// ======================== Build Profiles ========================

struct BuildProfile {
    std::string name;
    int maxFPS = 60;
    int renderScale = 100; // percentage
    bool useRayTracing = false;
    bool useMeshShaders = false;
    int shadowQuality = 2;
    int textureQuality = 2;
    int postProcessing = 2;
    int maxParticles = 100000;
    float drawDistance = 5000.0f;
    int maxAIUnits = 500;
    bool useHLOD = true;
    bool useOcclusionCulling = true;
    bool useVirtualTextures = false;
};

namespace BuildProfiles {
    inline BuildProfile Low() {
        return {"Low", 30, 75, false, false, 0, 0, 0, 25000, 2000.0f, 100, false, false, false};
    }
    inline BuildProfile Medium() {
        return {"Medium", 60, 100, false, false, 1, 1, 1, 50000, 3000.0f, 250, true, true, false};
    }
    inline BuildProfile High() {
        return {"High", 60, 100, false, false, 2, 2, 2, 100000, 5000.0f, 500, true, true, true};
    }
    inline BuildProfile Ultra() {
        return {"Ultra", 120, 100, true, true, 3, 3, 3, 200000, 8000.0f, 1000, true, true, true};
    }
    inline BuildProfile PS5() {
        return {"PS5", 60, 100, true, true, 3, 3, 3, 150000, 6000.0f, 750, true, true, true};
    }
    inline BuildProfile XboxSeriesX() {
        return {"Xbox Series X", 60, 100, true, true, 3, 3, 3, 150000, 6000.0f, 750, true, true, true};
    }
    inline BuildProfile Switch() {
        return {"Switch Handheld", 30, 75, false, false, 0, 0, 0, 25000, 2000.0f, 100, false, false, false};
    }
    inline BuildProfile SwitchDocked() {
        return {"Switch Docked", 30, 100, false, false, 1, 1, 1, 50000, 3000.0f, 200, false, false, false};
    }
}

} // namespace ogb
