#include "System/Assets/AssetRegistry.h"

#include <regex>

#include "System/FileSystem/FileHandler.h"

namespace {

static std::string ExtractJsonString(const std::string& json, const std::string& key)
{
	const std::regex expr("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]*)\\\"");
	std::smatch match;

	if (!std::regex_search(json, match, expr) || match.size() < 2) {
		return "";
	}

	return match[1].str();
}

static AssetDimension ParseDimension(const std::string& dimension)
{
	if (dimension == "2d") {
		return AssetDimension::TwoD;
	}

	if (dimension == "3d") {
		return AssetDimension::ThreeD;
	}

	return AssetDimension::Unknown;
}

static bool ParseAssetDescriptorJson(const std::string& json, AssetDescriptor& out)
{
	out.id = ExtractJsonString(json, "id");
	out.type = ExtractJsonString(json, "type");
	out.sourcePath = ExtractJsonString(json, "source");
	out.dimension = ParseDimension(ExtractJsonString(json, "dimension"));

	return !(out.id.empty() || out.sourcePath.empty());
}

} // namespace

namespace arclight {

bool AssetRegistry::RegisterManifest(const std::string& name, const AssetManifest& manifest)
{
	if (name.empty()) {
		return false;
	}

	manifests[name] = manifest;
	return true;
}

bool AssetRegistry::LoadFromDirectories(const std::string& name, const std::vector<std::string>& directories, const std::string& modes)
{
	if (name.empty()) {
		return false;
	}

	AssetManifest manifest;
	bool loadedAny = false;

	for (const std::string& directory : directories) {
		const std::vector<std::string> descriptorFiles = CFileHandler::DirList(directory, "*.json", modes);

		for (const std::string& descriptorPath : descriptorFiles) {
			CFileHandler file(descriptorPath, modes);
			if (!file.FileExists()) {
				continue;
			}

			std::string json;
			if (!file.LoadStringData(json)) {
				continue;
			}

			AssetDescriptor descriptor;
			if (!ParseAssetDescriptorJson(json, descriptor)) {
				continue;
			}

			loadedAny = manifest.AddAsset(descriptor) || loadedAny;
		}
	}

	if (loadedAny) {
		manifests[name] = manifest;
	}

	return loadedAny;
}

bool AssetRegistry::HasManifest(const std::string& name) const
{
	return manifests.find(name) != manifests.end();
}

const AssetManifest* AssetRegistry::GetManifest(const std::string& name) const
{
	auto iter = manifests.find(name);
	if (iter == manifests.end()) {
		return nullptr;
	}

	return &iter->second;
}

size_t AssetRegistry::GetAssetCount(const std::string& name) const
{
	const AssetManifest* manifest = GetManifest(name);
	if (manifest == nullptr) {
		return 0;
	}

	return manifest->GetAssets().size();
}

} // namespace arclight
