# ArcLight Engine - RTS World Subsystem
# Developer: Stephen
# Zone control, resource nodes, supply lines, territory management

set(WORLD_HEADERS
	RTSWorld.h
)

set(WORLD_SOURCES
	# Header-only for now
)

source_group("World" FILES ${WORLD_HEADERS} ${WORLD_SOURCES})

list(APPEND engineHeaders ${WORLD_HEADERS})
