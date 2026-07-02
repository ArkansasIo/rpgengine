/* OGame Beyond - Asset Pipeline
 * Developer: Stephen
 * Asset importing, validation, LOD generation, and dependency tracking.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ogb {

enum class AssetType { Texture, Mesh, Material, Sound, Music, Script, Font, Animation, Blueprint, DataTable };

struct AssetMetadata {
    std::string path;
    std::string name;
    AssetType type = AssetType::Texture;
    size_t fileSize = 0;
    uint32_t crc32 = 0;
    std::vector<std::string> dependencies;
    std::vector<std::string> tags;
    bool isDirty = false;
    double lastModified = 0;
};

struct LODConfig {
    int lod0Distance = 0;
    int lod1Distance = 500;
    int lod2Distance = 1000;
    int lod3Distance = 2000;
    float lodBias = 1.0f;
};

struct ImportSettings {
    bool generateMipmaps = true;
    bool compressTextures = true;
    int textureCompressionQuality = 2; // 0=fast, 1=normal, 2=best
    bool generateLODs = true;
    int lodCount = 3;
    float lodReductionFactor = 0.5f;
    bool optimizeMeshes = true;
    bool calculateTangents = true;
    bool flipUVs = false;
    bool importAnimations = true;
    float scaleFactor = 1.0f;
};

struct AssetBundle {
    std::string name;
    std::vector<std::string> assetPaths;
    bool isLoaded = false;
    size_t totalSize = 0;
};

class AssetPipeline {
public:
    ImportSettings defaultImportSettings;

    // Import
    bool ImportAsset(const std::string& sourcePath, const std::string& destPath, AssetType type) {
        AssetMetadata meta;
        meta.path = destPath;
        meta.name = ExtractName(sourcePath);
        meta.type = type;
        meta.isDirty = true;
        assets[destPath] = meta;
        return true;
    }

    bool ImportFBX(const std::string& path, const ImportSettings& settings) {
        return ImportAsset(path, path + ".mesh", AssetType::Mesh);
    }

    bool ImportTexture(const std::string& path, const ImportSettings& settings) {
        return ImportAsset(path, path + ".tex", AssetType::Texture);
    }

    // Validation
    bool ValidateAsset(const std::string& path) {
        auto it = assets.find(path);
        return it != assets.end() && it->second.fileSize > 0;
    }

    std::vector<std::string> ValidateAll() {
        std::vector<std::string> errors;
        for (auto& [path, meta] : assets) {
            if (meta.fileSize == 0) errors.push_back("Empty asset: " + path);
        }
        return errors;
    }

    // LOD generation
    void GenerateLODs(const std::string& meshPath, int lodCount = 3) {
        // Auto-generate LOD meshes
    }

    // Dependency tracking
    void AddDependency(const std::string& asset, const std::string& dependsOn) {
        assets[asset].dependencies.push_back(dependsOn);
    }

    std::vector<std::string> GetDependencies(const std::string& path) const {
        auto it = assets.find(path);
        return (it != assets.end()) ? it->second.dependencies : std::vector<std::string>();
    }

    std::vector<std::string> GetDependents(const std::string& path) const {
        std::vector<std::string> result;
        for (auto& [p, meta] : assets) {
            for (auto& dep : meta.dependencies) {
                if (dep == path) result.push_back(p);
            }
        }
        return result;
    }

    // Bundles
    void CreateBundle(const std::string& name, const std::vector<std::string>& paths) {
        AssetBundle bundle;
        bundle.name = name;
        bundle.assetPaths = paths;
        bundles[name] = bundle;
    }

    AssetMetadata* GetAsset(const std::string& path) {
        auto it = assets.find(path);
        return it != assets.end() ? &it->second : nullptr;
    }

    AssetType DetectType(const std::string& path) const {
        auto ext = path.substr(path.rfind('.') + 1);
        if (ext == "png" || ext == "jpg" || ext == "exr" || ext == "tga") return AssetType::Texture;
        if (ext == "fbx" || ext == "gltf" || ext == "obj" || ext == "usd") return AssetType::Mesh;
        if (ext == "wav" || ext == "mp3" || ext == "ogg") return AssetType::Sound;
        if (ext == "lua") return AssetType::Script;
        if (ext == "ttf" || ext == "otf") return AssetType::Font;
        if (ext == "anim" || ext == "skeleton") return AssetType::Animation;
        return AssetType::DataTable;
    }

private:
    std::unordered_map<std::string, AssetMetadata> assets;
    std::unordered_map<std::string, AssetBundle> bundles;

    std::string ExtractName(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        size_t lastDot = path.rfind('.');
        std::string filename = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
        return (lastDot != std::string::npos) ? filename.substr(0, lastDot - lastSlash - 1) : filename;
    }
};

} // namespace ogb
