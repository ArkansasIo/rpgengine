#ifndef ARCLIGHT_ASSET_MANIFEST_H
#define ARCLIGHT_ASSET_MANIFEST_H

#include <string>
#include <vector>

#include "AssetTypes.h"

namespace arclight {

class AssetManifest {
public:
	bool AddAsset(const AssetDescriptor& descriptor);
	const std::vector<AssetDescriptor>& GetAssets() const { return assets; }
	const AssetDescriptor* FindById(const std::string& id) const;

private:
	std::vector<AssetDescriptor> assets;
};

} // namespace arclight

#endif // ARCLIGHT_ASSET_MANIFEST_H
