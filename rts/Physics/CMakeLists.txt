# ArcLight Engine - Chaos-inspired Physics & Destruction
# Developer: Stephen
# UE5 Chaos-inspired rigid body physics and destruction

set(PHYSICS_HEADERS
	PhysicsSystem.h
	DestructionSystem.h
)

set(PHYSICS_SOURCES
	# Header-only for now
)

source_group("Physics" FILES ${PHYSICS_HEADERS} ${PHYSICS_SOURCES})

list(APPEND engineHeaders ${PHYSICS_HEADERS})
