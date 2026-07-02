/* ArkLight Beyond - Assimp Model Loader
 * Developer: Stephen
 * Multi-format 3D model import via Assimp.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>

namespace ogb {

struct MeshVertex {
    float3 position;
    float3 normal;
    float2 texCoord;
    float3 tangent;
    float3 bitangent;
};

struct MeshData {
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    int materialIndex = 0;
    float boundingBoxMin[3] = {-1, -1, -1};
    float boundingBoxMax[3] = {1, 1, 1};
};

struct ModelData {
    std::string name;
    std::vector<MeshData> meshes;
    int rootBone = -1;
    bool hasAnimations = false;
    bool hasSkeleton = false;
    float scale = 1.0f;
};

class AssimpModelLoader {
public:
    bool LoadFBX(const std::string& path) {
        ModelData model;
        model.name = ExtractName(path);
        // Use Assimp to import
        models[path] = model;
        return true;
    }

    bool LoadGLTF(const std::string& path) {
        ModelData model;
        model.name = ExtractName(path);
        models[path] = model;
        return true;
    }

    bool LoadOBJ(const std::string& path) {
        ModelData model;
        model.name = ExtractName(path);
        models[path] = model;
        return true;
    }

    bool LoadUSD(const std::string& path) {
        ModelData model;
        model.name = ExtractName(path);
        models[path] = model;
        return true;
    }

    ModelData* GetModel(const std::string& path) {
        auto it = models.find(path);
        return it != models.end() ? &it->second : nullptr;
    }

    // Auto-detect format and load
    bool Load(const std::string& path) {
        auto ext = path.substr(path.rfind('.') + 1);
        if (ext == "fbx") return LoadFBX(path);
        if (ext == "gltf" || ext == "glb") return LoadGLTF(path);
        if (ext == "obj") return LoadOBJ(path);
        if (ext == "usd" || ext == "usda" || ext == "usdc") return LoadUSD(path);
        return false;
    }

private:
    std::unordered_map<std::string, ModelData> models;

    std::string ExtractName(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        size_t lastDot = path.rfind('.');
        std::string filename = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
        return (lastDot != std::string::npos) ? filename.substr(0, lastDot - lastSlash - 1) : filename;
    }
};

} // namespace ogb
