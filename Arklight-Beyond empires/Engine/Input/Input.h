/* OGame Beyond - Input System
 * Developer: Stephen
 * Keyboard, mouse, gamepad input with action mapping.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace ogb {
struct InputAction {
    std::string name;
    int primaryKey = 0;
    int secondaryKey = 0;
    bool isPressed = false;
    bool wasPressed = false;
    std::function<void()> onPress;
    std::function<void()> onRelease;
};

struct MouseState {
    float x = 0, y = 0;
    float deltaX = 0, deltaY = 0;
    float scrollDelta = 0;
    bool leftButton = false;
    bool rightButton = false;
    bool middleButton = false;
    bool leftClicked = false;
    bool rightClicked = false;
};

struct GamepadState {
    bool connected = false;
    float leftX = 0, leftY = 0;
    float rightX = 0, rightY = 0;
    float leftTrigger = 0, rightTrigger = 0;
    bool buttons[16] = {};
};

class InputSystem {
public:
    void Init() {
        // Register default actions
        RegisterAction("CameraForward", 'W');
        RegisterAction("CameraBack", 'S');
        RegisterAction("CameraLeft", 'A');
        RegisterAction("CameraRight", 'D');
        RegisterAction("CameraRotateLeft", 'Q');
        RegisterAction("CameraRotateRight", 'E');
        RegisterAction("Select", 0x01); // Left mouse
        Command("Deselect", 0x1B); // Escape
        Command("Pause", 0x20); // Space
        Command("Minimap", 0x09); // Tab
    }

    void RegisterAction(const std::string& name, int key, std::function<void()> onPress = nullptr, std::function<void()> onRelease = nullptr) {
        InputAction action;
        action.name = name;
        action.primaryKey = key;
        action.onPress = std::move(onPress);
        action.onRelease = std::move(onRelease);
        actions[name] = action;
    }

    void Command(const std::string& name, int key) {
        InputAction action; action.name = name; action.primaryKey = key;
        actions[name] = action;
    }

    bool IsActionPressed(const std::string& name) const {
        auto it = actions.find(name);
        return it != actions.end() && it->second.isPressed;
    }

    bool IsActionJustPressed(const std::string& name) const {
        auto it = actions.find(name);
        return it != actions.end() && it->second.isPressed && !it->second.wasPressed;
    }

    void OnKeyDown(int key) {
        for (auto& [name, action] : actions) {
            if (action.primaryKey == key || action.secondaryKey == key) {
                action.wasPressed = action.isPressed;
                action.isPressed = true;
                if (action.onPress) action.onPress();
            }
        }
    }

    void OnKeyUp(int key) {
        for (auto& [name, action] : actions) {
            if (action.primaryKey == key || action.secondaryKey == key) {
                action.wasPressed = action.isPressed;
                action.isPressed = false;
                if (action.onRelease) action.onRelease();
            }
        }
    }

    void Update(float dt) {
        mouse.deltaX = 0;
        mouse.deltaY = 0;
        mouse.scrollDelta = 0;
        mouse.leftClicked = false;
        mouse.rightClicked = false;
    }

    const MouseState& GetMouse() const { return mouse; }
    void SetMousePos(float x, float y) { mouse.x = x; mouse.y = y; }
    void SetMouseButton(int btn, bool pressed) {
        if (btn == 0) { mouse.leftClicked = pressed && !mouse.leftButton; mouse.leftButton = pressed; }
        if (btn == 1) { mouse.rightClicked = pressed && !mouse.rightButton; mouse.rightButton = pressed; }
    }
    void SetMouseDelta(float dx, float dy) { mouse.deltaX = dx; mouse.deltaY = dy; }
    void SetScrollDelta(float d) { mouse.scrollDelta = d; }

    MouseState mouse;
    GamepadState gamepad;

private:
    std::unordered_map<std::string, InputAction> actions;
};
} // namespace ogb
