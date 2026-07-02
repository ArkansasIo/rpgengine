#pragma once

#include "../ScriptModule.h"
#include <string>

namespace arclight {

class GameScriptModule final : public ScriptModule {
public:
	explicit GameScriptModule(std::string scriptFile)
		: scriptPath(std::move(scriptFile)) {}

	ScriptModuleInfo GetInfo() const override {
		return {"GameScript", "1.0", "Gameplay Lua script module"};
	}

	void Load() override {
		loaded = true;
	}

	void Update(float /*dt*/) override {
		if (!loaded) {
			return;
		}
	}

	void Unload() override {
		loaded = false;
	}

	const std::string& GetScriptPath() const {
		return scriptPath;
	}

private:
	std::string scriptPath;
	bool loaded = false;
};

} // namespace arclight
