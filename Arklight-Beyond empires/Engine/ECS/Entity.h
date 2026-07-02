/* ArkLight Beyond - Entity Component System
 * Developer: Stephen
 * Core ECS with zero-overhead entity management.
 */
#pragma once
#include <cstdint>
#include <limits>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>
#include <functional>

namespace ogb {
using EntityID = uint32_t;
constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

class IComponent {
public:
    virtual ~IComponent() = default;
    EntityID owner = NULL_ENTITY;
    bool enabled = true;
    virtual const char* GetTypeName() const = 0;
};

class EntityManager {
public:
    EntityID Create();
    void Destroy(EntityID id);
    bool Valid(EntityID id) const;
    const std::vector<EntityID>& All() const { return entities; }

    template<typename T, typename... Args>
    T& Add(EntityID e, Args&&... args) {
        auto& store = GetStore<T>();
        auto c = std::make_unique<T>(std::forward<Args>(args)...);
        c->owner = e;
        T& ref = *c;
        store[e] = std::move(c);
        return ref;
    }

    template<typename T> void Remove(EntityID e) {
        auto it = stores.find(std::type_index(typeid(T)));
        if (it != stores.end()) it->second.erase(e);
    }

    template<typename T> T* Get(EntityID e) {
        auto it = stores.find(std::type_index(typeid(T)));
        if (it == stores.end()) return nullptr;
        auto c = it->second.find(e);
        return c != it->second.end() ? static_cast<T*>(c->second.get()) : nullptr;
    }

    template<typename T> bool Has(EntityID e) const {
        auto it = stores.find(std::type_index(typeid(T)));
        return it != stores.end() && it->second.count(e);
    }

    template<typename T> void ForEach(std::function<void(EntityID, T&)> fn) {
        auto it = stores.find(std::type_index(typeid(T)));
        if (it == stores.end()) return;
        for (auto& [id, comp] : it->second) fn(id, *static_cast<T*>(comp.get()));
    }

private:
    EntityID nextID = 1;
    std::vector<EntityID> entities;
    std::vector<EntityID> freeList;
    std::unordered_map<std::type_index,
        std::unordered_map<EntityID, std::unique_ptr<IComponent>>> stores;

    template<typename T>
    std::unordered_map<EntityID, std::unique_ptr<IComponent>>& GetStore() {
        return stores[std::type_index(typeid(T))];
    }
};
} // namespace ogb
