#ifndef ARCLIGHT_ASSET_REGISTRY_H
#define ARCLIGHT_ASSET_REGISTRY_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "AssetManifest.h"

namespace arclight {

class AssetRegistry {
public:
	bool RegisterManifest(const std::string& name, const AssetManifest& manifest);
	bool LoadFromDirectories(const std::string& name, const std::vector<std::string>& directories, const std::string& modes);
	bool HasManifest(const std::string& name) const;
	const AssetManifest* GetManifest(const std::string& name) const;
	size_t GetManifestCount() const { return manifests.size(); }
	size_t GetAssetCount(const std::string& name) const;

private:
	std::unordered_map<std::string, AssetManifest> manifests;
};

} // namespace arclight

#endif // ARCLIGHT_ASSET_REGISTRY_H
