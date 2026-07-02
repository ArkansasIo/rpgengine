# ArcLight Engine - Gameplay New Features
# Developer: Stephen
# Feature scaffolding for gameplay systems, scripting, and support logic.

add_subdirectory(ScriptingSystems)

set(GAMEPLAY_NEWFEATURE_HEADERS
	README.md
)

source_group("Gameplay/NewFeatures" FILES ${GAMEPLAY_NEWFEATURE_HEADERS})
list(APPEND engineHeaders ${GAMEPLAY_NEWFEATURE_HEADERS})
