add_library(engineaGui STATIC EXCLUDE_FROM_ALL
	Button.cpp
	Gui.cpp
	GuiElement.cpp
	HorizontalLayout.cpp
	LineEdit.cpp
	List.cpp
	Picture.cpp
	TextElement.cpp
	VerticalLayout.cpp
	Window.cpp
)
target_include_directories(engineaGui PRIVATE
		${CMAKE_SOURCE_DIR}/rts
		${SDL2_INCLUDE_DIRS}
		${GLEW_INCLUDE_DIR}
	PUBLIC
		${CMAKE_SOURCE_DIR}/rts/lib/slimsig/include
)
