# ArcLight Engine - ECS (Entity Component System)
# Developer: Stephen
# UE5-inspired component architecture

set(ECS_HEADERS
	Entity.h
	World.h
	Components/TransformComponent.h
	Components/HealthComponent.h
	Components/WeaponComponent.h
	Components/MovementComponent.h
	Components/FogOfWarComponent.h
	Components/SelectionComponent.h
	Components/FormationComponent.h
	Components/InventoryComponent.h
)

set(ECS_SOURCES
	# Header-only for now - template-heavy ECS
)

source_group("ECS" FILES ${ECS_HEADERS} ${ECS_SOURCES})
source_group("ECS/Components" FILES
	Components/TransformComponent.h
	Components/HealthComponent.h
	Components/WeaponComponent.h
	Components/MovementComponent.h
	Components/FogOfWarComponent.h
	Components/SelectionComponent.h
	Components/FormationComponent.h
	Components/InventoryComponent.h
)

list(APPEND engineHeaders ${ECS_HEADERS})
