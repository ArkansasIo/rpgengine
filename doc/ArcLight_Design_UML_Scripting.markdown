# ArcLight Engine Design, UML, and Scripting Guide

This document gives a compact overview of how the engine is structured, how
the major source-code subsystems relate to one another, and how to write
gameplay logic in Lua with an object-oriented layer.

## 1. Source-Code Design

ArcLight is split into clear engine layers:

- `rts/System/` for filesystem, logging, platform, startup, and shared runtime services
- `rts/Game/` for gameplay systems, UI, settings, and flow control
- `rts/ECS/` for component-style gameplay data and world management
- `rts/Lua/` for Lua integration, parser helpers, and runtime bindings
- `rts/Rendering/` for graphics, textures, shaders, and UI rendering
- `rts/Sim/` for simulation, units, weapons, movement, and game rules
- `cont/` for packaged content, scripts, and assets

The code should stay separated by responsibility. Engine code controls runtime
systems, while content and scripts control game behavior.

### Recommended source layout

```text
rts/
  System/     startup, assets, plugins, file loading
  Game/       gameplay systems, UI, settings, blueprints
  ECS/        entities, components, worlds
  Lua/        Lua runtime and bindings
  Rendering/  graphics and presentation
  Sim/        deterministic simulation
cont/
  base/       shipped content archive
  freedesktop/ desktop launcher assets
```

## 2. UML Overview

The following class diagram shows the current high-level relationships for the
new asset, plugin, blueprint, and Lua scripting layers.

```mermaid
classDiagram
    class SpringApp {
      +Init()
      +Run()
      +Reload()
    }

    class AssetRegistry {
      +RegisterManifest(name, manifest)
      +LoadFromDirectories(name, directories, modes)
      +GetManifest(name)
    }

    class AssetManifest {
      +AddAsset(descriptor)
      +FindById(id)
      +GetAssets()
    }

    class AssetDescriptor {
      +id
      +type
      +sourcePath
      +dimension
    }

    class PluginManager {
      +RegisterPlugin(plugin)
      +LoadFromDirectory(directory, modes)
      +Shutdown()
    }

    class IEnginePlugin {
      <<interface>>
      +GetName()
      +OnLoad()
      +OnUnload()
    }

    class BlueprintSystem {
      +CreateGraph(name)
      +CompileGraph(graph)
      +ExecuteGraph(graph)
    }

    class BlueprintGraph {
      +AddNode(type, name, x, y)
      +Connect(fromNode, fromPin, toNode, toPin)
      +RemoveNode(nodeID)
    }

    class BlueprintCompiler {
      +Compile(graph)
      +CompileNode(graph, node)
    }

    class BlueprintRuntime {
      +Load(code)
      +Execute(graph)
      +SetVariable(name, value)
    }

    class Runtime {
      +registerSystem(name, system)
      +start(ctx)
      +update(dt, ctx)
      +shutdown(ctx)
    }

    SpringApp --> AssetRegistry
    SpringApp --> PluginManager
    SpringApp --> BlueprintSystem
    AssetRegistry --> AssetManifest
    AssetManifest --> AssetDescriptor
    PluginManager --> IEnginePlugin
    BlueprintSystem --> BlueprintGraph
    BlueprintSystem --> BlueprintCompiler
    BlueprintSystem --> BlueprintRuntime
    Runtime --> "Lua systems"
```

## 3. Runtime Flow

1. `SpringApp` initializes filesystem and runtime services.
2. `PluginManager` scans plugin manifests and loads Lua-backed plugins.
3. `AssetRegistry` scans asset descriptor JSON files and builds runtime manifests.
4. Gameplay systems create or load `BlueprintGraph` objects when visual scripting is used.
5. Lua scripts run through `Runtime`, which provides a small object-oriented layer.

## 4. Lua Scripting Style

ArcLight scripting stays in Lua, but the engine expects scripts to be structured
like OOP code instead of loose globals.

### Example class layer

```lua
local OOScript = VFS.Include("scripts/ooscript/ooclass.lua")
local Runtime = VFS.Include("scripts/ooscript/runtime.lua")

local class = OOScript.class

local Component = class("Component")
function Component:init(name)
  self.name = name
end

local PlayerMovement = class("PlayerMovement", Component)
function PlayerMovement:init(speed)
  Component.init(self, "PlayerMovement")
  self.speed = speed or 6.0
end

function PlayerMovement:onUpdate(dt, ctx)
  self.distance = (self.distance or 0) + (self.speed * dt)
end

Runtime.registerSystem("PlayerMovement", PlayerMovement(7.5))
return Runtime
```

### Script rules

- Prefer module tables over globals.
- Use `init`, `onStart`, `onUpdate`, and `onShutdown` as lifecycle methods.
- Keep gameplay logic in scripts and engine-only state in C++.
- Use JSON descriptors for assets and plugin metadata.

## 5. Engine Coding Guidelines

- Keep headers small and focused.
- Put runtime logic in `.cpp` files when it is not purely data.
- Prefer `std::vector`, `std::unordered_map`, and `std::unique_ptr` for ownership
  and collection management.
- Avoid mixing rendering, simulation, and file loading in the same class.
- Design new systems so they can be initialized from `SpringApp` without special
  per-feature code paths.

## 6. Practical Example

If you add a new gameplay feature, implement it in this order:

1. Add the core C++ system class in `rts/Game/` or `rts/System/`.
2. Add a simple data descriptor in `cont/base/springcontent/` if the feature needs content.
3. Add a Lua wrapper or runtime script if the feature needs gameplay-side control.
4. Document the feature with a short UML note or class diagram before expanding it.

This keeps the engine understandable as it grows.