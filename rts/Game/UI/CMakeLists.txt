# ArcLight Engine - UI Systems
# Developer: Stephen

set(UI_HEADERS
	Stellaris/StellarisUI.h
	Editor/UE5EditorUI.h
)

source_group("Game/UI" FILES ${UI_HEADERS})
list(APPEND engineHeaders ${UI_HEADERS})
