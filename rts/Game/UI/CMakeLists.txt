# ArcLight Engine - Game UI
# Developer: Stephen

set(UI_HEADERS
	Stellaris/StellarisUI.h
	Editor/UE5EditorUI.h
	SpaceHUD/SpaceHUD.h
	SpaceHUD/SpaceHUDLayout.h
	SpaceHUD/SpaceHUDTheme.h
	SpaceHUD/SpaceHUDTopBar.h
	SpaceHUD/SpaceHUDSideRail.h
	SpaceHUD/SpaceHUDBottomDeck.h
	SpaceHUD/SpaceHUDGalaxyPanel.h
	SpaceHUD/SpaceHUDFleetPanel.h
	SpaceHUD/SpaceHUDResearchPanel.h
)

source_group("Game/UI" FILES ${UI_HEADERS})
list(APPEND engineHeaders ${UI_HEADERS})
