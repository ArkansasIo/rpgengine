# ArcLight Engine - Game Systems
# Developer: Stephen

set(GAME_SYSTEMS_HEADERS
	SaveLoadSystem.h
	AchievementSystem.h
	QuestSystem.h
	FactionSystem.h
	TradeSystem.h
	CraftingSystem.h
)

source_group("Game/Systems" FILES ${GAME_SYSTEMS_HEADERS})
list(APPEND engineHeaders ${GAME_SYSTEMS_HEADERS})
