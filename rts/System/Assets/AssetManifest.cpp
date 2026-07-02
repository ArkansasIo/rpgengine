#include "System/Assets/AssetManifest.h"

namespace arclight {

bool AssetManifest::AddAsset(const AssetDescriptor& descriptor)
{
	if (descriptor.id.empty() || descriptor.sourcePath.empty()) {
		return false;
	}

	if (FindById(descriptor.id) != nullptr) {
		return false;
	}

	assets.push_back(descriptor);
	return true;
}

const AssetDescriptor* AssetManifest::FindById(const std::string& id) const
{
	for (const AssetDescriptor& asset : assets) {
		if (asset.id == id) {
			return &asset;
		}
	}

	return nullptr;
}

} // namespace arclight
