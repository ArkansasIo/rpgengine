#pragma once

#include "../ScriptModule.h"
#include <string>

namespace arclight {

class EngineLifecycleScriptModule final : public ScriptModule {
public:
	ScriptModuleInfo GetInfo() const override {
		return {"EngineLifecycle", "1.0", "Engine startup, update, and shutdown hook module"};
	}

	void Load() override {
		loaded = true;
	}

	void Update(float dt) override {
		if (!loaded) {
			return;
		}

		lastDeltaTime = dt;
		frameCount += 1;
	}

	void Unload() override {
		loaded = false;
	}

	unsigned int GetFrameCount() const {
		return frameCount;
	}

	float GetLastDeltaTime() const {
		return lastDeltaTime;
	}

private:
	bool loaded = false;
	unsigned int frameCount = 0;
	float lastDeltaTime = 0.0f;
};

} // namespace arclight
