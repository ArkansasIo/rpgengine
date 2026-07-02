#pragma once

#include "../ScriptModule.h"
#include <memory>
#include <vector>

namespace arclight {

class ScriptRuntime {
public:
	void RegisterModule(std::unique_ptr<ScriptModule> module) {
		modules.push_back(std::move(module));
	}

	void LoadAll() {
		for (auto& module : modules) {
			if (module) {
				module->Load();
			}
		}
	}

	void UpdateAll(float dt) {
		for (auto& module : modules) {
			if (module) {
				module->Update(dt);
			}
		}
	}

	void UnloadAll() {
		for (auto& module : modules) {
			if (module) {
				module->Unload();
			}
		}
		modules.clear();
	}

	size_t GetModuleCount() const {
		return modules.size();
	}

private:
	std::vector<std::unique_ptr<ScriptModule>> modules;
};

} // namespace arclight
