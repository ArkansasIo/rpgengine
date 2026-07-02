# ArcLight Engine - UE5-inspired Behavior Tree
# Developer: Stephen

set(BT_HEADERS
	BehaviorTree.h
)

set(BT_SOURCES
	# Header-only for now
)

source_group("AI/BehaviorTree" FILES ${BT_HEADERS} ${BT_SOURCES})

list(APPEND engineHeaders ${BT_HEADERS})
