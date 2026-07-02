# ArcLight Engine - Gameplay Scripting Systems
# Developer: Stephen
# Lua OOP helpers, runtime bridge, and game scripting scaffolding.

set(SCRIPTING_HEADERS
	README.md
	OOScript/ooclass.lua
	OOScript/runtime.lua
	GameScripts/example_game.lua
	GameScripts/GameScriptModule.h
	Docs/UML.md
	Docs/HowToCode.md
	ScriptModule.h
	Runtime/ScriptRuntime.h
	Runtime/EngineLifecycleScriptModule.h
	OOScript/OOScript.h
)

source_group("Gameplay/NewFeatures/ScriptingSystems" FILES ${SCRIPTING_HEADERS})
list(APPEND engineHeaders ${SCRIPTING_HEADERS})
