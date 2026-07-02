# Plugin System

Drop plugin manifests and scripts here.

Each plugin should contain:
- plugin.json: metadata and entry point
- plugin.lua: Lua bootstrap/runtime hook

Engine side loading is implemented in rts/System/Plugins.
