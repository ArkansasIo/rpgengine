#ifndef ARCLIGHT_PLUGIN_MANAGER_H
#define ARCLIGHT_PLUGIN_MANAGER_H

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

#include "IEnginePlugin.h"

namespace arclight {

class PluginManager {
public:
	bool RegisterPlugin(std::unique_ptr<IEnginePlugin> plugin);
	bool LoadFromDirectory(const std::string& directory, const std::string& modes);
	bool UnregisterPlugin(const std::string& name);
	bool IsLoaded(const std::string& name) const;
	size_t GetLoadedCount() const { return plugins.size(); }
	void Shutdown();

private:
	std::unordered_map<std::string, std::unique_ptr<IEnginePlugin>> plugins;
};

} // namespace arclight

#endif // ARCLIGHT_PLUGIN_MANAGER_H
