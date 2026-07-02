#ifndef ARCLIGHT_ASSET_TYPES_H
#define ARCLIGHT_ASSET_TYPES_H

#include <string>

namespace arclight {

enum class AssetDimension {
	Unknown = 0,
	TwoD,
	ThreeD
};

struct AssetDescriptor {
	std::string id;
	std::string type;
	std::string sourcePath;
	AssetDimension dimension = AssetDimension::Unknown;
};

} // namespace arclight

#endif // ARCLIGHT_ASSET_TYPES_H
