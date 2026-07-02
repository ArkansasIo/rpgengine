# ArcLight Engine API Reference
**Developer: Stephen** | Version 1.0.0

---

## Table of Contents

1. [ECS (Entity Component System)](#ecs)
2. [Gameplay Ability System](#gameplay-ability-system)
3. [Niagara Particle System](#niagara-particle-system)
4. [Chaos Physics & Destruction](#chaos-physics--destruction)
5. [Behavior Trees](#behavior-trees)
6. [RTS World System](#rts-world-system)
7. [Faction System](#faction-system)
8. [Trade System](#trade-system)
9. [Quest System](#quest-system)
10. [Crafting System](#crafting-system)
11. [Save/Load System](#saveload-system)
12. [Menu System](#menu-system)
13. [Settings & Options](#settings--options)
14. [Stellaris UI](#stellaris-ui)
15. [UE5 Editor UI](#ue5-editor-ui)
16. [Blueprint System](#blueprint-system)

---

## ECS

### EntityManager

```cpp
namespace arclight {

class EntityManager {
    EntityID CreateEntity();
    void DestroyEntity(EntityID id);
    bool IsValid(EntityID id) const;

    template<typename T, typename... Args>
    T& AddComponent(EntityID entity, Args&&... args);

    template<typename T>
    void RemoveComponent(EntityID entity);

    template<typename T>
    T* GetComponent(EntityID entity);

    template<typename T>
    bool HasComponent(EntityID entity) const;

    template<typename T>
    void ForEachComponent(std::function<void(EntityID, T&)> func);
};

}
```

### World

```cpp
class World {
    EntityManager& GetEntityManager();
    EntityID SpawnEntity();
    void DestroyEntity(EntityID entity);

    template<typename T, typename... Args>
    T& AddSystem(Args&&... args);

    void InitSystems();
    void UpdateSystems(float deltaTime);

    void TagEntity(EntityID entity, const std::string& tag);
    std::vector<EntityID> GetEntitiesWithTag(const std::string& tag);
};
```

### Components

| Component | Key Members |
|-----------|-------------|
| `TransformComponent` | position, rotation, scale, parent, GetForward(), GetRight() |
| `HealthComponent` | maxHealth, currentHealth, maxShield, armor, ApplyDamage(), Heal() |
| `WeaponComponent` | damage, range, fireRate, fireMode, criticalChance, CanFire() |
| `MovementComponent` | maxSpeed, acceleration, velocity, targetPosition, pathWaypoints |
| `FogOfWarComponent` | sightRadius, radarRange, isCloaked, GetVisibility() |
| `SelectionComponent` | state (Selected/Hovered), isSelectable, groupID |
| `InventoryComponent` | slots[40], AddItem(), RemoveItem(), EquipItem() |

---

## Gameplay Ability System

### GameplayAbility

```cpp
class GameplayAbility {
    std::string abilityName;
    AbilityID id;
    AbilityCooldown cooldown;
    float duration; // -1 = indefinite

    bool CanActivate() const;
    void Activate();
    void Deactivate();
    void Tick(float dt);
    void Cancel();
};
```

### AbilitySystemComponent

```cpp
class AbilitySystemComponent {
    void AddAbility(std::unique_ptr<GameplayAbility> ability);
    bool TryActivateAbility(AbilityID id);
    void DeactivateAbility(AbilityID id);
    void Update(float dt);
    bool HasActiveAbility() const;
    GameplayAbility* FindAbilityByName(const std::string& name);
};
```

---

## Niagara Particle System

### NiagaraSystem

```cpp
class NiagaraSystem {
    void AddModule(std::unique_ptr<IEmitterModule> module);
    void Update(float dt);
    void Render();
    void Reset();
    size_t GetActiveParticleCount() const;
};
```

### Built-in Modules

| Module | Purpose |
|--------|---------|
| `SpawnRateModule` | Spawn particles at a continuous rate |
| `BurstModule` | Spawn a burst of particles |
| `ParticleInitModule` | Set initial position, velocity, lifetime |
| `GravityModule` | Apply gravitational acceleration |
| `SizeOverLifetimeModule` | Animate size over particle lifetime |
| `ColorOverLifetimeModule` | Animate color gradient over lifetime |
| `AlphaOverLifetimeModule` | Animate transparency over lifetime |
| `CurlNoiseModule` | Apply turbulence displacement |
| `KillDeadModule` | Remove expired particles |

---

## Chaos Physics & Destruction

### PhysicsSystem

```cpp
class PhysicsSystem {
    void SetGravity(const float3& gravity);
    int AddBody(const RigidBody& body);
    void RemoveBody(int index);
    RigidBody* GetBody(int index);
    void Update(float dt);
};
```

### DestructionSystem

```cpp
class DestructionSystem {
    void SetParams(const DestructionParams& params);
    std::vector<FractureChunk> Fracture(
        const float3& center,
        const float3& size,
        const DestructionEvent& event
    );
    void Update(float dt, std::vector<FractureChunk>& chunks);
};
```

### Fracture Modes

| Mode | Description |
|------|-------------|
| `Voronoi` | Voronoi-based realistic shattering |
| `Planar` | Clean planar cuts |
| `Radial` | Radial explosion pattern |
| `Clustered` | Clustered chunk generation |

---

## Behavior Trees

### Node Types

| Type | Class | Purpose |
|------|-------|---------|
| Selector | `SelectorNode` | Try children left-to-right, succeed on first success |
| Sequence | `SequenceNode` | Execute children left-to-right, fail on first failure |
| Parallel | `ParallelNode` | Execute all children simultaneously |
| Condition | `ConditionDecorator` | Check a condition before executing child |
| Task | `TaskNode` | Leaf node with lambda execution |

### Blackboard

```cpp
class Blackboard {
    void SetValue(const std::string& key, float value);
    void SetValue(const std::string& key, int value);
    void SetValue(const std::string& key, bool value);
    void SetValue(const std::string& key, const std::string& value);
    float GetFloat(const std::string& key, float default = 0) const;
    int GetInt(const std::string& key, int default = 0) const;
    bool GetBool(const std::string& key, bool default = false) const;
};
```

---

## RTS World System

```cpp
class RTSWorld {
    int CreateZone(const float3& center, float radius, ZoneType type = Neutral);
    void ConnectZones(int zoneA, int zoneB);
    void CaptureZone(int zoneID, int teamID);
    int CreateResourceNode(const float3& position, float amount, int type = 0);
    ResourceNode* GetNearestResourceNode(const float3& position, float maxRange = 5000);
    std::vector<int> FindPath(int fromZone, int toZone);
    float GetTeamControlPercent(int teamID) const;
};
```

---

## Faction System

```cpp
class FactionSystem {
    void RegisterFaction(const FactionDef& def);
    void ChangeReputation(const std::string& factionID, float amount);
    void SetDiplomacy(const std::string& from, const std::string& to, DiplomacyState state);
    DiplomacyState GetDiplomacy(const std::string& from, const std::string& to) const;
    bool AreAllied(const std::string& a, const std::string& b) const;
};
```

### Diplomacy States

`Hostile` | `Unfriendly` | `Neutral` | `Friendly` | `Allied`

---

## Trade System

```cpp
class TradeSystem {
    void RegisterResource(const ResourceType& res);
    float GetMarketPrice(const std::string& resourceID) const;
    void UpdateSupplyDemand(const std::string& resourceID, float supplyDelta, float demandDelta);
    float ExecuteTrade(const std::string& resourceID, int quantity, bool isBuying, const std::string& factionID);
    void Update(float dt); // market fluctuation
};
```

---

## Quest System

```cpp
class QuestSystem {
    void RegisterQuest(const QuestDef& def);
    bool AcceptQuest(const std::string& questID);
    void UpdateObjective(const std::string& questID, const std::string& objectiveID, int amount = 1);
    void CompleteQuest(const std::string& questID);
    QuestStatus GetQuestStatus(const std::string& questID) const;
    std::vector<QuestDef> GetActiveQuests() const;
};
```

---

## Crafting System

```cpp
class CraftingSystem {
    void RegisterRecipe(const CraftingRecipe& recipe);
    bool LearnRecipe(const std::string& recipeID);
    bool CanCraft(const std::string& recipeID, int playerLevel, ...);
    void StartCrafting(const std::string& recipeID);
    void Update(float dt);
};
```

---

## Save/Load System

```cpp
class SaveLoadSystem {
    bool SaveGame(int slot, const std::string& name = "");
    bool LoadGame(int slot);
    void Autosave();
    void DeleteSave(int slot);
    const SaveMetadata& GetSaveInfo(int slot) const;
    void Update(float dt);
};
```

---

## Menu System

```cpp
class MenuSystem {
    void Init();
    void ChangeState(MenuState newState);
    void Update(float dt);
    void Render();
    void OnMouseClick(float x, float y);
    MenuState GetCurrentState() const;
};
```

### Menu States

`MainMenu` | `NewGame` | `LoadGame` | `Multiplayer` | `Settings` | `PauseMenu` | `InGameMenu` | `Exit`

---

## Settings & Options

### GraphicsSettings

```cpp
struct GraphicsSettings {
    bool fullscreen, vsync;
    int resolutionX, resolutionY;
    int shadowQuality, textureQuality, antiAliasing;
    bool bloom, ambientOcclusion, screenSpaceReflections;
    float drawDistance, gamma, brightness;
    void ApplyPreset(int preset); // 0=Low, 1=Med, 2=High, 3=Ultra
};
```

### AudioSettings

```cpp
struct AudioSettings {
    float masterVolume, musicVolume, sfxVolume;
    bool muteAll;
    std::string language;
};
```

### InputSettings

```cpp
struct InputSettings {
    float mouseSensitivity;
    bool invertMouseY;
    float cameraRotationSpeed, cameraPanSpeed;
    bool edgeScrolling;
};
```

---

## Stellaris UI

```cpp
class StellarisUI {
    StellarisTopBar topBar;        // Empire resources
    StellarisOutliner outliner;    // Right panel
    StellarisSidePanel sidePanel;  // Context-sensitive
    StellarisGalaxyMap galaxyMap;  // Map overlay
    StellarisTooltip tooltip;      // Contextual tooltips
    StellarisNotificationSystem notifications;

    void Init();
    void Update(float dt);
    void OpenPlanetView(const std::string& planetName);
    void OpenFleetView(const std::string& fleetName);
    void OpenDiplomacy(const std::string& empireName);
};
```

---

## UE5 Editor UI

```cpp
class UE5EditorUI {
    EditorViewport viewport;
    EditorDetailsPanel detailsPanel;
    EditorWorldOutliner worldOutliner;
    EditorContentBrowser contentBrowser;
    EditorToolbar toolbar;
    EditorCommandPalette commandPalette;

    void Init();
    void Update(float dt);
    void SelectObject(const std::string& id);
    void ToggleCommandPalette();
};
```

---

## Blueprint System

```cpp
class BlueprintSystem {
    BlueprintGraph* CreateGraph(const std::string& name);
    bool CompileGraph(BlueprintGraph& graph);
    void ExecuteGraph(BlueprintGraph& graph);
    BlueprintGraph* FindGraph(const std::string& name);
};

class BlueprintGraph {
    BlueprintNode* AddNode(EBlueprintNodeType type, const std::string& name, float x, float y);
    BlueprintConnection* Connect(const std::string& fromNode, const std::string& fromPin,
                                  const std::string& toNode, const std::string& toPin);
    void AddVariable(const BlueprintVariable& var);
};
```

### Node Types

`Entry` | `Event` | `Branch` | `Print` | `Math` | `SetVariable` | `GetVariable` | `ForEach`

---

## Quick Start

```cpp
#include "ArcLightEngine.h"

using namespace arclight;

int main() {
    ArcLightEngine engine;
    engine.Init();

    // Spawn a unit
    EntityID unit = engine.SpawnUnit(float3(0, 0, 0), 0);

    // Game loop
    while (running) {
        float dt = GetDeltaTime();
        engine.Update(dt);
        Render();
    }

    engine.Shutdown();
    return 0;
}
```

---

*Generated by ArcLight Engine Build System | Developer: Stephen*
