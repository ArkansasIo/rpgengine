/* OGame Beyond - Renderer
 * Developer: Stephen
 * Modern deferred/hybrid renderer with PBR, GI, and post-processing.
 */
#pragma once
#include "../Core/Types.h"
#include "../Core/UObject.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace ogb {
struct RenderConfig {
    int screenWidth = 1920;
    int screenHeight = 1080;
    bool fullscreen = false;
    bool vsync = true;
    float renderScale = 1.0f;
    int shadowQuality = 2; // 0=off, 1=low, 2=med, 3=high, 4=ultra
    bool usePBR = true;
    bool useGI = true;
    bool useBloom = true;
    bool useMotionBlur = false;
    bool useDOF = false;
    bool useSSR = true;
    bool useAO = true;
    bool useRayTracing = false;
    int antiAliasing = 2; // 0=off, 1=FXAA, 2=MSAA2x, 3=MSAA4x, 4=TAA
    float drawDistance = 5000;
    int maxParticles = 100000;
};

struct Light {
    Vec3 position;
    Color color;
    float intensity = 1.0f;
    float radius = 100.0f;
    bool isDirectional = false;
    Vec3 direction;
};

struct RenderObject {
    int objectID = -1;
    Vec3 position;
    Vec3 rotation;
    float scale = 1.0f;
    int meshID = -1;
    int materialID = -1;
    bool isVisible = true;
    bool castShadows = true;
    float lodBias = 1.0f;
};

class Renderer {
public:
    RenderConfig config;
    std::vector<Light> lights;
    std::vector<RenderObject> objects;

    void Init() {
        // Initialize graphics API
        screenWidth = config.screenWidth;
        screenHeight = config.screenHeight;
    }

    void BeginFrame() {
        // Clear buffers, set camera
    }

    void RenderScene() {
        // Depth pre-pass
        // Shadow maps
        // Deferred G-buffer
        // Lighting pass
        // Forward pass for transparent
        // Post-processing
    }

    void EndFrame() {
        // Swap buffers
    }

    void Resize(int w, int h) {
        screenWidth = w;
        screenHeight = h;
        config.screenWidth = w;
        config.screenHeight = h;
    }

    int AddLight(const Light& light) {
        int id = static_cast<int>(lights.size());
        lights.push_back(light);
        return id;
    }

    int AddObject(const RenderObject& obj) {
        int id = static_cast<int>(objects.size());
        objects.push_back(obj);
        return id;
    }

    void SetCamera(const Vec3& pos, const Vec3& target, float fov = 60) {
        cameraPos = pos;
        cameraTarget = target;
        cameraFOV = fov;
    }

    void DrawText(float x, float y, const std::string& text, const Color& color, float scale = 1.0f) {
        // Text rendering
    }

    void DrawRect(float x, float y, float w, float h, const Color& color) {
        // Rectangle rendering
    }

    void DrawLine(float x1, float y1, float x2, float y2, const Color& color, float width = 1) {
        // Line rendering
    }

    void DrawCircle(float x, float y, float r, const Color& color, float width = 1) {
        // Circle rendering
    }

    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }

private:
    int screenWidth = 1920;
    int screenHeight = 1080;
    Vec3 cameraPos;
    Vec3 cameraTarget;
    float cameraFOV = 60;
};
} // namespace ogb
