/* ArcLight Engine - Menu System
 * Developer: Stephen
 * UE5-inspired menu framework with state machine, transitions, and widget support.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace arclight {

enum class MenuState {
	MainMenu,
	NewGame,
	LoadGame,
	Multiplayer,
	Settings,
	Achievements,
	Credits,
	Exit,
	PauseMenu,
	InGameMenu,
	LoadingScreen,
};

enum class WidgetType {
	Button,
	Label,
	Slider,
	Checkbox,
	Dropdown,
	TextInput,
	Image,
	Panel,
	ProgressBar,
};

struct MenuItem {
	std::string id;
	std::string text;
	std::string tooltip;
	WidgetType type = WidgetType::Button;
	bool isVisible = true;
	bool isEnabled = true;
	float x = 0, y = 0, width = 200, height = 40;
	std::function<void()> onClick;
};

class IMenuScreen {
public:
	virtual ~IMenuScreen() = default;
	virtual void Enter() {}
	virtual void Exit() {}
	virtual void Update(float dt) {}
	virtual void Render() {}
	virtual void OnKeyPress(int key) {}
	virtual void OnMouseClick(float x, float y) {}
	virtual void OnMouseMove(float x, float y) {}
};

class MainMenuScreen : public IMenuScreen {
public:
	void Enter() override {
		items.clear();
		float yPos = 300.0f;
		float xCenter = 500.0f;

		auto addBtn = [&](const std::string& id, const std::string& text, std::function<void()> cb) {
			MenuItem item;
			item.id = id;
			item.text = text;
			item.x = xCenter - 150;
			item.y = yPos;
			item.width = 300;
			item.height = 50;
			item.onClick = std::move(cb);
			items.push_back(item);
			yPos += 70;
		};

		addBtn("new_game", "New Game", [this]() { requestStateChange(MenuState::NewGame); });
		addBtn("load_game", "Load Game", [this]() { requestStateChange(MenuState::LoadGame); });
		addBtn("multiplayer", "Multiplayer", [this]() { requestStateChange(MenuState::Multiplayer); });
		addBtn("settings", "Settings", [this]() { requestStateChange(MenuState::Settings); });
		addBtn("achievements", "Achievements", [this]() { requestStateChange(MenuState::Achievements); });
		addBtn("credits", "Credits", [this]() { requestStateChange(MenuState::Credits); });
		addBtn("exit", "Exit", [this]() { requestStateChange(MenuState::Exit); });
	}

	void OnMouseClick(float mx, float my) override {
		for (auto& item : items) {
			if (item.isVisible && item.isEnabled &&
				mx >= item.x && mx <= item.x + item.width &&
				my >= item.y && my <= item.y + item.height) {
				if (item.onClick) item.onClick();
			}
		}
	}

	std::vector<MenuItem> items;
	std::function<void(MenuState)> requestStateChange;
};

class PauseMenuScreen : public IMenuScreen {
public:
	void Enter() override {
		items.clear();
		float yPos = 350.0f;
		float xCenter = 500.0f;

		auto addBtn = [&](const std::string& id, const std::string& text, std::function<void()> cb) {
			MenuItem item;
			item.id = id;
			item.text = text;
			item.x = xCenter - 150;
			item.y = yPos;
			item.width = 300;
			item.height = 50;
			item.onClick = std::move(cb);
			items.push_back(item);
			yPos += 70;
		};

		addBtn("resume", "Resume", [this]() { requestStateChange(MenuState::InGameMenu); });
		addBtn("save", "Save Game", [this]() { requestStateChange(MenuState::LoadGame); });
		addBtn("settings", "Settings", [this]() { requestStateChange(MenuState::Settings); });
		addBtn("quit", "Quit to Menu", [this]() { requestStateChange(MenuState::MainMenu); });
	}

	void OnMouseClick(float mx, float my) override {
		for (auto& item : items) {
			if (item.isVisible && item.isEnabled &&
				mx >= item.x && mx <= item.x + item.width &&
				my >= item.y && my <= item.y + item.height) {
				if (item.onClick) item.onClick();
			}
		}
	}

	std::vector<MenuItem> items;
	std::function<void(MenuState)> requestStateChange;
};

class MenuSystem {
public:
	void Init() {
		mainMenu = std::make_unique<MainMenuScreen>();
		pauseMenu = std::make_unique<PauseMenuScreen>();

		mainMenu->requestStateChange = [this](MenuState s) { ChangeState(s); };
		pauseMenu->requestStateChange = [this](MenuState s) { ChangeState(s); };

		screens[MenuState::MainMenu] = mainMenu.get();
		screens[MenuState::PauseMenu] = pauseMenu.get();

		ChangeState(MenuState::MainMenu);
	}

	void ChangeState(MenuState newState) {
		if (currentState == newState) return;
		if (currentScreen) currentScreen->Exit();
		currentState = newState;
		currentScreen = nullptr;
		auto it = screens.find(newState);
		if (it != screens.end()) {
			currentScreen = it->second;
			currentScreen->Enter();
		}
		if (onStateChanged) onStateChanged(newState);
	}

	void Update(float dt) {
		if (currentScreen) currentScreen->Update(dt);
	}

	void Render() {
		if (currentScreen) currentScreen->Render();
	}

	void OnMouseClick(float x, float y) {
		if (currentScreen) currentScreen->OnMouseClick(x, y);
	}

	void OnKeyPress(int key) {
		if (currentScreen) currentScreen->OnKeyPress(key);
	}

	void RegisterScreen(MenuState state, IMenuScreen* screen) {
		screens[state] = screen;
	}

	MenuState GetCurrentState() const { return currentState; }

	std::function<void(MenuState)> onStateChanged;

private:
	MenuState currentState = MenuState::MainMenu;
	IMenuScreen* currentScreen = nullptr;
	std::unordered_map<MenuState, IMenuScreen*> screens;
	std::unique_ptr<MainMenuScreen> mainMenu;
	std::unique_ptr<PauseMenuScreen> pauseMenu;
};

} // namespace arclight
