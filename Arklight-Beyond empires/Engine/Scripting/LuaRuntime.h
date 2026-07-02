/* ArkLight Beyond - Lua Runtime
 * Developer: Stephen
 * Lua scripting integration with engine API exposure.
 */
#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace ogb {
class World;

struct LuaScript {
    std::string id;
    std::string filePath;
    std::string source;
    bool isLoaded = false;
    bool isActive = true;
};

class LuaRuntime {
public:
    void Init() {}
    void Shutdown() { scripts.clear(); }

    bool LoadScript(const std::string& id, const std::string& path) {
        LuaScript script;
        script.id = id;
        script.filePath = path;
        script.isLoaded = true;
        scripts[id] = script;
        return true;
    }

    bool ExecuteString(const std::string& code) {
        // Execute Lua code string
        return true;
    }

    void CallFunction(const std::string& funcName, const std::vector<std::string>& args = {}) {
        auto it = registeredFunctions.find(funcName);
        if (it != registeredFunctions.end()) {
            it->second(args);
        }
    }

    void RegisterFunction(const std::string& name, std::function<void(const std::vector<std::string>&)> fn) {
        registeredFunctions[name] = std::move(fn);
    }

    void Update(float dt) {
        for (auto& [id, script] : scripts) {
            if (script.isActive && script.isLoaded) {
                CallFunction("OnUpdate", {std::to_string(dt)});
            }
        }
    }

    void ExposeAPI() {
        // Expose engine functions to Lua
        RegisterFunction("GetGameFrame", [](const std::vector<std::string>&) {});
        RegisterFunction("GetTeamList", [](const std::vector<std::string>&) {});
        RegisterFunction("GetUnitPosition", [](const std::vector<std::string>&) {});
        RegisterFunction("SendCommands", [](const std::vector<std::string>&) {});
        RegisterFunction("Echo", [](const std::vector<std::string>& args) {
            for (auto& a : args) printf("[Lua] %s\n", a.c_str());
        });
    }

private:
    std::unordered_map<std::string, LuaScript> scripts;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> registeredFunctions;
};
} // namespace ogb
