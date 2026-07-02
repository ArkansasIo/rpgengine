#pragma once

#include <string>

namespace arclight {

struct ScriptModuleInfo {
	std::string name;
	std::string version;
	std::string description;
};

class ScriptModule {
public:
	virtual ~ScriptModule() = default;
	virtual ScriptModuleInfo GetInfo() const = 0;
	virtual void Load() = 0;
	virtual void Update(float /*dt*/) {}
	virtual void Unload() = 0;
};

} // namespace arclight
