#include "System/Plugins/PluginManager.h"

#include <regex>

#include "System/FileSystem/FileHandler.h"
#include "System/Log/ILog.h"

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

class ScriptPlugin final : public arclight::IEnginePlugin {
public:
	ScriptPlugin(std::string pluginName, std::string pluginEntry)
		: name(std::move(pluginName))
		, entry(std::move(pluginEntry))
	{}

	const char* GetName() const override { return name.c_str(); }

	bool OnLoad() override
	{
		LOG("[PluginManager] Loaded plugin '%s' (entry: %s)", name.c_str(), entry.c_str());
		return true;
	}

	void OnUnload() override
	{
		LOG("[PluginManager] Unloaded plugin '%s'", name.c_str());
	}

private:
	std::string name;
	std::string entry;
};

} // namespace

namespace arclight {

bool PluginManager::RegisterPlugin(std::unique_ptr<IEnginePlugin> plugin)
{
	if (plugin == nullptr) {
		return false;
	}

	const std::string name = plugin->GetName();
	if (name.empty() || plugins.find(name) != plugins.end()) {
		return false;
	}

	if (!plugin->OnLoad()) {
		return false;
	}

	plugins.emplace(name, std::move(plugin));
	return true;
}

bool PluginManager::LoadFromDirectory(const std::string& directory, const std::string& modes)
{
	const std::vector<std::string> manifestFiles = CFileHandler::DirList(directory, "*.json", modes);
	bool loadedAny = false;

	for (const std::string& manifestPath : manifestFiles) {
		CFileHandler file(manifestPath, modes);
		if (!file.FileExists()) {
			continue;
		}

		std::string json;
		if (!file.LoadStringData(json)) {
			continue;
		}

		const std::string name = ExtractJsonString(json, "name");
		const std::string entry = ExtractJsonString(json, "entry");

		if (name.empty()) {
			LOG_L(L_WARNING, "[PluginManager] Ignoring plugin manifest without name: %s", manifestPath.c_str());
			continue;
		}

		if (RegisterPlugin(std::make_unique<ScriptPlugin>(name, entry))) {
			loadedAny = true;
		}
	}

	return loadedAny;
}

bool PluginManager::UnregisterPlugin(const std::string& name)
{
	auto iter = plugins.find(name);
	if (iter == plugins.end()) {
		return false;
	}

	iter->second->OnUnload();
	plugins.erase(iter);
	return true;
}

bool PluginManager::IsLoaded(const std::string& name) const
{
	return plugins.find(name) != plugins.end();
}

void PluginManager::Shutdown()
{
	for (auto& entry : plugins) {
		entry.second->OnUnload();
	}

	plugins.clear();
}

} // namespace arclight
