/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired Entity Component System
 * Entities are lightweight IDs that own Components.
 * Unlike UE5's UObject-based Actors, these are pure data containers
 * with zero virtual dispatch overhead.
 */

#pragma once

#include <cstdint>
#include <limits>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>

namespace arclight {

using EntityID = uint32_t;
constexpr EntityID INVALID_ENTITY = std::numeric_limits<EntityID>::max();

class IComponent {
public:
	virtual ~IComponent() = default;
	EntityID owner = INVALID_ENTITY;
	bool enabled = true;
};

class EntityManager {
public:
	EntityID CreateEntity() {
		EntityID id;
		if (!freeList.empty()) {
			id = freeList.back();
			freeList.pop_back();
		} else {
			id = nextID++;
		}
		entities.push_back(id);
		return id;
	}

	void DestroyEntity(EntityID id) {
		for (auto& [type, store] : componentStores) {
			store.erase(id);
		}
		entities.erase(
			std::remove(entities.begin(), entities.end(), id),
			entities.end()
		);
		freeList.push_back(id);
	}

	bool IsValid(EntityID id) const {
		return std::find(entities.begin(), entities.end(), id) != entities.end();
	}

	const std::vector<EntityID>& GetAllEntities() const { return entities; }

	template<typename T, typename... Args>
	T& AddComponent(EntityID entity, Args&&... args) {
		auto& store = GetOrCreateStore<T>();
		auto comp = std::make_unique<T>(std::forward<Args>(args)...);
		comp->owner = entity;
		T& ref = *comp;
		store[entity] = std::move(comp);
		return ref;
	}

	template<typename T>
	void RemoveComponent(EntityID entity) {
		auto it = componentStores.find(std::type_index(typeid(T)));
		if (it != componentStores.end()) {
			it->second.erase(entity);
		}
	}

	template<typename T>
	T* GetComponent(EntityID entity) {
		auto it = componentStores.find(std::type_index(typeid(T)));
		if (it == componentStores.end()) return nullptr;
		auto& store = it->second;
		auto cit = store.find(entity);
		if (cit == store.end()) return nullptr;
		return static_cast<T*>(cit->second.get());
	}

	template<typename T>
	const T* GetComponent(EntityID entity) const {
		auto it = componentStores.find(std::type_index(typeid(T)));
		if (it == componentStores.end()) return nullptr;
		auto& store = it->second;
		auto cit = store.find(entity);
		if (cit == store.end()) return nullptr;
		return static_cast<const T*>(cit->second.get());
	}

	template<typename T>
	bool HasComponent(EntityID entity) const {
		auto it = componentStores.find(std::type_index(typeid(T)));
		if (it == componentStores.end()) return false;
		return it->second.find(entity) != it->second.end();
	}

	template<typename T>
	void ForEachComponent(std::function<void(EntityID, T&)> func) {
		auto it = componentStores.find(std::type_index(typeid(T)));
		if (it == componentStores.end()) return;
		for (auto& [entity, comp] : it->second) {
			func(entity, *static_cast<T*>(comp.get()));
		}
	}

private:
	template<typename T>
	std::unordered_map<EntityID, std::unique_ptr<IComponent>>& GetOrCreateStore() {
		auto key = std::type_index(typeid(T));
		return componentStores[key];
	}

	EntityID nextID = 1;
	std::vector<EntityID> entities;
	std::vector<EntityID> freeList;
	std::unordered_map<
		std::type_index,
		std::unordered_map<EntityID, std::unique_ptr<IComponent>>
	> componentStores;
};

} // namespace arclight
