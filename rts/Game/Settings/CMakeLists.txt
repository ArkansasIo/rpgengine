# ArcLight Engine - Settings & Options
# Developer: Stephen

set(SETTINGS_HEADERS
	GameSettings.h
	KeyBindings.h
	DisplaySettings.h
	UISettings.h
)

source_group("Game/Settings" FILES ${SETTINGS_HEADERS})
list(APPEND engineHeaders ${SETTINGS_HEADERS})
