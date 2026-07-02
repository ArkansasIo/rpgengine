/* ArkLight Beyond - Script Modules
 * Developer: Stephen
 * Game script lifecycle modules for Lua integration.
 */
#pragma once
#include <string>
#include <vector>
#include <functional>

namespace ogb {

class IScriptModule {
public:
    virtual ~IScriptModule() = default;
    virtual void Init() {}
    virtual void Shutdown() {}
    virtual void Update(float dt) {}
    virtual const char* GetName() const = 0;
};

class ScriptModuleManager {
public:
    void RegisterModule(std::unique_ptr<IScriptModule> module) {
        modules.push_back(std::move(module));
    }

    void InitAll() { for (auto& m : modules) m->Init(); }
    void UpdateAll(float dt) { for (auto& m : modules) m->Update(dt); }
    void ShutdownAll() { for (auto& m : modules) m->Shutdown(); modules.clear(); }

    IScriptModule* GetModule(const std::string& name) {
        for (auto& m : modules) if (std::string(m->GetName()) == name) return m.get();
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<IScriptModule>> modules;
};

class GameScriptModule : public IScriptModule {
public:
    const char* GetName() const override { return "GameScript"; }
    void Init() override { printf("[Script] GameScript module loaded\n"); }
    void Update(float dt) override {}
};

class EngineLifecycleModule : public IScriptModule {
public:
    const char* GetName() const override { return "EngineLifecycle"; }
    void Init() override { printf("[Script] EngineLifecycle module loaded\n"); }
    void Shutdown() override { printf("[Script] EngineLifecycle module shutdown\n"); }
};

} // namespace ogb
