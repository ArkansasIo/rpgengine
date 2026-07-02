# ArcLight Engine - Gameplay Ability System
# Developer: Stephen
# UE5-inspired GAS (Gameplay Ability System)

set(GAMEPLAY_HEADERS
	GameplayAbility.h
	AbilitySystemComponent.h
)

set(GAMEPLAY_SOURCES
	# Header-only for now
)

source_group("Gameplay" FILES ${GAMEPLAY_HEADERS} ${GAMEPLAY_SOURCES})

list(APPEND engineHeaders ${GAMEPLAY_HEADERS})
