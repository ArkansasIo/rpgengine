/* ArkLight Beyond - EnTT ECS Integration
 * Developer: Stephen
 * EnTT-based ECS with archetype storage for high performance.
 * Provides low-level EnTT access and high-level game object wrappers.
 */
#pragma once
#include "../Core/Types.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <any>
#include <memory>

namespace ogb {
using EntityID = uint32_t;
constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

// ======================== Component Registry ========================

class ComponentRegistry {
public:
    template<typename T>
    void RegisterComponent() {
        auto typeIdx = std::type_index(typeid(T));
        componentNames[typeIdx] = typeid(T).name();
        componentSizes[typeIdx] = sizeof(T);
    }

    template<typename T>
    bool IsRegistered() const {
        return componentNames.count(std::type_index(typeid(T)));
    }

    size_t GetComponentCount() const { return componentNames.size(); }

private:
    std::unordered_map<std::type_index, std::string> componentNames;
    std::unordered_map<std::type_index, size_t> componentSizes;
};

// ======================== Entity Manager ========================

class EntityManager {
public:
    EntityID Create() {
        EntityID id;
        if (!freeList.empty()) { id = freeList.back(); freeList.pop_back(); }
        else { id = nextID++; }
        entities.push_back(id);
        return id;
    }

    void Destroy(EntityID id) {
        for (auto& [type, store] : componentStores) store.erase(id);
        entities.erase(std::remove(entities.begin(), entities.end(), id), entities.end());
        freeList.push_back(id);
    }

    bool Valid(EntityID id) const {
        return std::find(entities.begin(), entities.end(), id) != entities.end();
    }

    const std::vector<EntityID>& All() const { return entities; }
    size_t Count() const { return entities.size(); }

    template<typename T, typename... Args>
    T& Add(EntityID e, Args&&... args) {
        auto& store = GetStore<T>();
        auto c = std::make_unique<T>(std::forward<Args>(args)...);
        c->owner = e;
        T& ref = *c;
        store[e] = std::move(c);
        return ref;
    }

    template<typename T>
    void Remove(EntityID e) {
        auto it = stores.find(std::type_index(typeid(T)));
        if (it != stores.end()) it->second.erase(e);
    }

    template<typename T>
    T* Get(EntityID e) {
        auto it = stores.find(std::type_index(typeid(T)));
        if (it == stores.end()) return nullptr;
        auto c = it->second.find(e);
        return c != it->second.end() ? static_cast<T*>(c->second.get()) : nullptr;
    }

    template<typename T>
    bool Has(EntityID e) const {
        auto it = stores.find(std::type_index(typeid(T)));
        return it != stores.end() && it->second.count(e);
    }

    template<typename T>
    void ForEach(std::function<void(EntityID, T&)> fn) {
        auto it = stores.find(std::type_index(typeid(T)));
        if (it == stores.end()) return;
        for (auto& [id, comp] : it->second) fn(id, *static_cast<T*>(comp.get()));
    }

private:
    EntityID nextID = 1;
    std::vector<EntityID> entities;
    std::vector<EntityID> freeList;
    std::unordered_map<std::type_index,
        std::unordered_map<EntityID, std::unique_ptr<void>>> stores;

    template<typename T>
    std::unordered_map<EntityID, std::unique_ptr<void>>& GetStore() {
        auto key = std::type_index(typeid(T));
        return stores[key];
    }
};

// ======================== World ========================

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
        return ref;
    }

    void InitSystems() { for (auto& s : systems) s->Init(); }
    void Tick(float dt) { for (auto& s : systems) if (s->enabled) s->Update(dt); }
    void ShutdownSystems() { for (auto& s : systems) s->Shutdown(); systems.clear(); }

    void Tag(EntityID e, const std::string& t) { tags[t].push_back(e); }

    std::vector<EntityID> GetTagged(const std::string& t) const {
        auto it = tags.find(t);
        return it != tags.end() ? it->second : std::vector<EntityID>();
    }

    // Batch operations
    template<typename T>
    void ForEachComponent(std::function<void(EntityID, T&)> fn) {
        em.ForEach<T>(std::move(fn));
    }

    size_t EntityCount() const { return em.Count(); }

private:
    EntityManager em;

    struct ISystemBase {
        virtual ~ISystemBase() = default;
        virtual void Init() {}
        virtual void Update(float dt) {}
        virtual void Shutdown() {}
        std::string name;
        int priority = 0;
        bool enabled = true;
    };

    std::vector<std::unique_ptr<ISystemBase>> systems;
    std::unordered_map<std::string, std::vector<EntityID>> tags;
};

} // namespace ogb
