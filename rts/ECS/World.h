/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * UE5-inspired World - top-level container for all entities and systems.
 * Manages entity lifecycle, component queries, and system tick ordering.
 */

#pragma once

#include "Entity.h"
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>

namespace arclight {

class ISystem {
public:
	virtual ~ISystem() = default;
	virtual void Init() {}
	virtual void Update(float /*deltaTime*/) {}
	virtual void Shutdown() {}
	std::string name;
	int priority = 0; // lower = runs first
	bool enabled = true;
};

class World {
public:
	EntityManager& GetEntityManager() { return entityManager; }
	const EntityManager& GetEntityManager() const { return entityManager; }

	EntityID SpawnEntity() {
		return entityManager.CreateEntity();
	}

	void DestroyEntity(EntityID entity) {
		entityManager.DestroyEntity(entity);
	}

	template<typename T, typename... Args>
	T& AddSystem(Args&&... args) {
		auto system = std::make_unique<T>(std::forward<Args>(args)...);
		T& ref = *system;
		systems.push_back(std::move(system));
		std::sort(systems.begin(), systems.end(),
			[](const auto& a, const auto& b) {
				return a->priority < b->priority;
			}
		);
		return ref;
	}

	template<typename T>
	T* GetSystem() {
		for (auto& sys : systems) {
			T* cast = dynamic_cast<T*>(sys.get());
			if (cast) return cast;
		}
		return nullptr;
	}

	void InitSystems() {
		for (auto& sys : systems) {
			sys->Init();
		}
	}

	void UpdateSystems(float deltaTime) {
		for (auto& sys : systems) {
			if (sys->enabled) {
				sys->Update(deltaTime);
			}
		}
	}

	void ShutdownSystems() {
		for (auto& sys : systems) {
			sys->Shutdown();
		}
		systems.clear();
	}

	// Tag system - attach string tags to entities for快速 queries
	void TagEntity(EntityID entity, const std::string& tag) {
		tagToEntities[tag].push_back(entity);
		entityToTags[entity].push_back(tag);
	}

	std::vector<EntityID> GetEntitiesWithTag(const std::string& tag) const {
		auto it = tagToEntities.find(tag);
		if (it != tagToEntities.end()) return it->second;
		return {};
	}

	bool EntityHasTag(EntityID entity, const std::string& tag) const {
		auto it = entityToTags.find(entity);
		if (it == entityToTags.end()) return false;
		return std::find(it->second.begin(), it->second.end(), tag) != it->second.end();
	}

private:
	EntityManager entityManager;
	std::vector<std::unique_ptr<ISystem>> systems;
	std::unordered_map<std::string, std::vector<EntityID>> tagToEntities;
	std::unordered_map<EntityID, std::vector<std::string>> entityToTags;
};

} // namespace arclight
