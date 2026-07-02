# ArcLight Engine - Grand Strategy / 4X / MMO foundations

set(GRAND_STRATEGY_HEADERS
	Types.h
	Galaxy.h
	Empire.h
	Economy.h
	Fleet.h
	Simulation.h
)

set(GRAND_STRATEGY_SOURCES
	Galaxy.cpp
)

add_library(arclightGrandStrategy STATIC EXCLUDE_FROM_ALL
	${GRAND_STRATEGY_HEADERS}
	${GRAND_STRATEGY_SOURCES}
)

target_include_directories(arclightGrandStrategy
	PUBLIC
		${CMAKE_SOURCE_DIR}/rts
)

if(ARCLIGHT_CXX_STANDARD GREATER_EQUAL 23)
	target_compile_features(arclightGrandStrategy PUBLIC cxx_std_23)
elseif(ARCLIGHT_CXX_STANDARD EQUAL 20)
	target_compile_features(arclightGrandStrategy PUBLIC cxx_std_20)
else()
	target_compile_features(arclightGrandStrategy PUBLIC cxx_std_17)
endif()

source_group("GrandStrategy" FILES ${GRAND_STRATEGY_HEADERS} ${GRAND_STRATEGY_SOURCES})

list(APPEND engineHeaders ${GRAND_STRATEGY_HEADERS})
