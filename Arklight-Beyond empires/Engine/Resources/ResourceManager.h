/* OGame Beyond - Resource Manager
 * Developer: Stephen
 * Asset loading, caching, and lifecycle management.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace ogb {
enum class AssetType { Texture, Mesh, Material, Sound, Music, Script, Font, Animation };

struct AssetHandle {
    uint64_t id = 0;
    AssetType type = AssetType::Texture;
    std::string path;
    bool isLoaded = false;
    size_t memorySize = 0;
};

class ResourceManager {
public:
    AssetHandle LoadAsset(const std::string& path, AssetType type) {
        AssetHandle handle;
        handle.id = nextAssetID++;
        handle.path = path;
        handle.type = type;
        handle.isLoaded = true;
        assets[handle.id] = handle;
        return handle;
    }

    void UnloadAsset(uint64_t id) {
        assets.erase(id);
    }

    AssetHandle* GetAsset(uint64_t id) {
        auto it = assets.find(id);
        return it != assets.end() ? &it->second : nullptr;
    }

    AssetHandle* FindAsset(const std::string& path) {
        for (auto& [id, asset] : assets) {
            if (asset.path == path) return &asset;
        }
        return nullptr;
    }

    void PreloadAssets(const std::vector<std::string>& paths, AssetType type) {
        for (auto& p : paths) LoadAsset(p, type);
    }

    size_t GetTotalMemoryUsage() const {
        size_t total = 0;
        for (auto& [id, asset] : assets) total += asset.memorySize;
        return total;
    }

    int GetLoadedAssetCount() const { return static_cast<int>(assets.size()); }

    void GarbageCollect() {
        // Remove unreferenced assets
    }

private:
    std::unordered_map<uint64_t, AssetHandle> assets;
    uint64_t nextAssetID = 1;
};
} // namespace ogb
