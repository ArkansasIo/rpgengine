/* ArkLight Beyond - World Manager
 * Developer: Stephen
 * Top-level container for all entities, systems, and tags.
 */
#pragma once
#include "Entity.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {
class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void Init() {}
    virtual void Update(float dt) {}
    virtual void Shutdown() {}
    std::string name;
    int priority = 0;
    bool enabled = true;
};

class World {
public:
    EntityManager& EM() { return em; }

    EntityID Spawn() { return em.Create(); }
    void Destroy(EntityID e) { em.Destroy(e); }

    template<typename T, typename... Args>
    T& AddSystem(Args&&... args) {
        auto sys = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *sys;
        systems.push_back(std::move(sys));
        std::sort(systems.begin(), systems.end(),
            [](auto& a, auto& b) { return a->priority < b->priority; });
        return ref;
    }

    void InitSystems() { for (auto& s : systems) s->Init(); }
    void Tick(float dt) { for (auto& s : systems) if (s->enabled) s->Update(dt); }
    void ShutdownSystems() { for (auto& s : systems) s->Shutdown(); systems.clear(); }

    void Tag(EntityID e, const std::string& t) { tags[t].push_back(e); entityTags[e].push_back(t); }
    std::vector<EntityID> GetTagged(const std::string& t) const {
        auto it = tags.find(t); return it != tags.end() ? it->second : std::vector<EntityID>();
    }

private:
    EntityManager em;
    std::vector<std::unique_ptr<ISystem>> systems;
    std::unordered_map<std::string, std::vector<EntityID>> tags;
    std::unordered_map<EntityID, std::vector<std::string>> entityTags;
};
} // namespace ogb
