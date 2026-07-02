/* OGame Beyond - Master Engine Header
 * Developer: Stephen
 * Single include for the entire OGameBeyond engine.
 */
#pragma once

// Core
#include "Core/Types.h"
#include "Core/UObject.h"
#include "Core/AActor.h"
#include "Core/UActorComponent.h"
#include "Core/APawn.h"
#include "Core/ACharacter.h"
#include "Core/AController.h"
#include "Core/APlayerController.h"
#include "Core/AAIController.h"
#include "Core/AGameModeBase.h"
#include "Core/GameTime.h"
#include "Core/EventBus.h"
#include "Core/Platform.h"
#include "Core/PlatformIO.h"
#include "Core/PlatformBuildConfig.h"

// ECS
#include "ECS/Entity.h"
#include "ECS/EnTTWorld.h"

// Renderer
#include "Renderer/VulkanRenderer.h"

// Audio
#include "Audio/OpenALAudio.h"

// Input
#include "Input/Input.h"

// Physics
#include "Physics/JoltPhysics.h"

// Networking
#include "Networking/ENetNetworking.h"

// Scripting
#include "Scripting/LuaRuntime.h"

// SaveGame
#include "SaveGame/SaveGame.h"

// Resources
#include "Resources/ResourceManager.h"
#include "Resources/AssetPipeline.h"
#include "Resources/ModelLoader.h"
#include "Resources/FreeTypeFont.h"

// UI
#include "UI/ImGuiEditor.h"

// Game Systems
#include "../Game/Universe/Universe.h"
#include "../Game/Galaxy/Galaxy.h"
#include "../Game/Economy/Resources.h"
#include "../Game/Economy/Economy.h"
#include "../Game/Buildings/Buildings.h"
#include "../Game/Ships/Ships.h"
#include "../Game/Fleets/Fleet.h"
#include "../Game/Battles/Battles.h"
#include "../Game/Research/Research.h"
#include "../Game/Diplomacy/Diplomacy.h"
#include "../Game/Colonies/Colonies.h"
#include "../Game/AI/EmpireAI.h"

namespace ogb {

class OGameBeyondEngine {
public:
    // Engine Systems
    World world;
    GameTime gameTime;
    EventBus eventBus;
    PlatformLayer platform;
    VulkanRendererLowLevel vulkanLow;
    RendererHighLevel renderer;
    OpenALAudioEngine audio;
    InputSystem input;
    JoltPhysicsSystem physics;
    ENetNetworkManager network;
    LuaRuntime lua;
    SaveSystem saveSystem;
    ResourceManager resources;
    AssetPipeline assetPipeline;
    FreeTypeFontSystem fonts;
    ImGuiEditor editor;
    PlatformSaveSystem platformSave;
    PlatformTrophySystem trophies;
    GamepadManager gamepads;

    // Game Systems
    Universe universe;
    Galaxy galaxy;
    ResourceManager resourcesManager;
    EconomySystem economy;
    BuildingRegistry buildingRegistry;
    ShipRegistry shipRegistry;
    FleetManager fleetManager;
    BattleSystem battleSystem;
    ResearchRegistry researchRegistry;
    DiplomacySystem diplomacy;
    ColonyManager colonyManager;
    std::vector<std::unique_ptr<EmpireAI>> empireAIs;

    void Init() {
        platform.Init({});
        world.InitSystems();
        gameTime.Init();
        audio.Init();
        input.Init();
        editor.Init();
        resourcesManager.Init();
        buildingRegistry.Init();
        shipRegistry.Init();
        researchRegistry.Init();
        galaxy.Generate(200, 5000);
        trophies.Init();
        gamepads.Init();
        lua.ExposeAPI();
        printf("[OGameBeyond] Engine initialized\n");
    }

    void Update(float dt) {
        gameTime.Update(dt);
        world.Tick(dt);
        physics.Update(dt);
        input.Update(dt);
        audio.Update(dt);
        gamepads.Update(dt);
        economy.Update(dt);
        fleetManager.Update(dt);
        battleSystem.Update(dt);
        colonyManager.Update(dt);
        saveSystem.Update(dt);
        for (auto& ai : empireAIs) ai->Update(dt);
        lua.Update(dt);
    }

    void Shutdown() {
        world.ShutdownSystems();
        audio.Shutdown();
        lua.Shutdown();
        printf("[OGameBeyond] Engine shutdown\n");
    }

    void Render() {
        renderer.BeginFrame();
        renderer.SetCamera(float3(0, 500, -800), float3(0, 0, 0), 60);
        renderer.RenderScene();
        renderer.RenderUI();
        renderer.EndFrame();
    }
};

} // namespace ogb
