/* This file is part of the ArcLight Engine
 * Developer: Stephen
 *
 * Master include header for all subsystems.
 * Include this single header to access the full ArcLight feature set.
 *
 * Subsystems:
 *   - ECS:          Entity Component System (UE5 Actor-Component pattern)
 *   - Gameplay:     Gameplay Ability System (UE5 GAS)
 *   - Particle:     Niagara-inspired GPU particle system
 *   - Physics:      Chaos-inspired rigid body & destruction
 *   - AI:           UE5 Behavior Tree system
 *   - World:        RTS zone control, resources, supply lines
 *   - Systems:      Save/Load, Achievements, Quests, Factions, Trade, Crafting
 *   - Menus:        Main menu, pause menu, screen transitions
 *   - Settings:     Graphics, Audio, Input, Gameplay, Display, UI, Key Bindings
 */

#pragma once

// ======================== ECS (Entity Component System) ========================
#include "ECS/Entity.h"
#include "ECS/World.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/HealthComponent.h"
#include "ECS/Components/WeaponComponent.h"
#include "ECS/Components/MovementComponent.h"
#include "ECS/Components/FogOfWarComponent.h"
#include "ECS/Components/SelectionComponent.h"
#include "ECS/Components/FormationComponent.h"
#include "ECS/Components/InventoryComponent.h"

// ======================== Gameplay Ability System ========================
#include "Gameplay/GameplayAbility.h"
#include "Gameplay/AbilitySystemComponent.h"

// ======================== Niagara Particle System ========================
#include "Particle/NiagaraSystem.h"

// ======================== Chaos Physics & Destruction ========================
#include "Physics/PhysicsSystem.h"
#include "Physics/DestructionSystem.h"

// ======================== AI Behavior Tree ========================
#include "AI/BehaviorTree/BehaviorTree.h"

// ======================== RTS World ========================
#include "World/RTSWorld.h"

// ======================== Game Systems ========================
#include "Game/Systems/SaveLoadSystem.h"
#include "Game/Systems/AchievementSystem.h"
#include "Game/Systems/QuestSystem.h"
#include "Game/Systems/FactionSystem.h"
#include "Game/Systems/TradeSystem.h"
#include "Game/Systems/CraftingSystem.h"

// ======================== Menu System ========================
#include "Game/Menus/MenuSystem.h"

// ======================== Settings & Options ========================
#include "Game/Settings/GameSettings.h"
#include "Game/Settings/KeyBindings.h"
#include "Game/Settings/DisplaySettings.h"
#include "Game/Settings/UISettings.h"

namespace arclight {

/**
 * ArcLight Engine Master System
 * Integrates all subsystems into a cohesive engine.
 */
class ArcLightEngine {
public:
	// Core
	World ecsWorld;
	RTSWorld rtsWorld;
	PhysicsSystem physicsSystem;
	DestructionSystem destructionSystem;

	// Game Systems
	SaveLoadSystem saveLoadSystem;
	AchievementSystem achievementSystem;
	QuestSystem questSystem;
	FactionSystem factionSystem;
	TradeSystem tradeSystem;
	CraftingSystem craftingSystem;

	// Menu & UI
	MenuSystem menuSystem;

	// Settings
	GameSettings gameSettings;
	KeyBindingSystem keyBindings;
	DisplaySettings displaySettings;
	UISettings uiSettings;

	void Init() {
		ecsWorld.InitSystems();
		physicsSystem.SetGravity(float3(0.0f, -9.81f, 0.0f));
		destructionSystem.SetParams(DestructionParams());
		menuSystem.Init();
		keyBindings.SetDefaultBindings();
		displaySettings.Init();
		gameSettings.Init();
	}

	void Update(float dt) {
		ecsWorld.UpdateSystems(dt);
		physicsSystem.Update(dt);
		rtsWorld.UpdateResources(dt);
		saveLoadSystem.Update(dt);
		menuSystem.Update(dt);
	}

	void Shutdown() {
		ecsWorld.ShutdownSystems();
	}

	void TogglePause() {
		if (menuSystem.GetCurrentState() == MenuState::PauseMenu)
			menuSystem.ChangeState(MenuState::InGameMenu);
		else
			menuSystem.ChangeState(MenuState::PauseMenu);
	}

	void OpenSettings() {
		menuSystem.ChangeState(MenuState::Settings);
	}

	void ReturnToMainMenu() {
		menuSystem.ChangeState(MenuState::MainMenu);
	}

	// Convenience: spawn an RTS entity with all core components
	EntityID SpawnUnit(const float3& position, int teamID) {
		EntityID entity = ecsWorld.SpawnEntity();

		auto& transform = ecsWorld.GetEntityManager().AddComponent<TransformComponent>(entity);
		transform.SetPosition(position);

		auto& health = ecsWorld.GetEntityManager().AddComponent<HealthComponent>(entity);
		health.maxHealth = 100.0f;
		health.currentHealth = 100.0f;

		auto& movement = ecsWorld.GetEntityManager().AddComponent<MovementComponent>(entity);
		movement.maxSpeed = 100.0f;

		auto& selection = ecsWorld.GetEntityManager().AddComponent<SelectionComponent>(entity);

		auto& fog = ecsWorld.GetEntityManager().AddComponent<FogOfWarComponent>(entity);
		fog.sightRadius = 500.0f;

		ecsWorld.TagEntity(entity, "team_" + std::to_string(teamID));
		ecsWorld.TagEntity(entity, "unit");

		return entity;
	}

	// Convenience: spawn a building entity
	EntityID SpawnBuilding(const float3& position, int teamID) {
		EntityID entity = ecsWorld.SpawnEntity();

		auto& transform = ecsWorld.GetEntityManager().AddComponent<TransformComponent>(entity);
		transform.SetPosition(position);

		auto& health = ecsWorld.GetEntityManager().AddComponent<HealthComponent>(entity);
		health.maxHealth = 500.0f;
		health.currentHealth = 500.0f;
		health.armor = 10.0f;

		auto& selection = ecsWorld.GetEntityManager().AddComponent<SelectionComponent>(entity);

		ecsWorld.TagEntity(entity, "team_" + std::to_string(teamID));
		ecsWorld.TagEntity(entity, "building");

		return entity;
	}

	// Convenience: spawn a projectile
	EntityID SpawnProjectile(
		const float3& position,
		const float3& velocity,
		float damage,
		EntityID owner
	) {
		EntityID entity = ecsWorld.SpawnEntity();

		auto& transform = ecsWorld.GetEntityManager().AddComponent<TransformComponent>(entity);
		transform.SetPosition(position);

		auto& movement = ecsWorld.GetEntityManager().AddComponent<MovementComponent>(entity);
		movement.velocity = velocity;
		movement.movementMode = MovementMode::Flying;

		RigidBody body;
		body.position = position;
		body.velocity = velocity;
		body.mass = 0.1f;
		body.collisionRadius = 2.0f;
		body.entityID = static_cast<int>(entity);
		body.useGravity = false;
		physicsSystem.AddBody(body);

		ecsWorld.TagEntity(entity, "projectile");

		return entity;
	}
};

} // namespace arclight
