/* OGame Beyond - APlayerController
 * Developer: Stephen
 * Player-specific controller with input, HUD, and camera management.
 */
#pragma once
#include "AController.h"

namespace ogb {
class UPlayerInput;

struct InputState {
    float mouseX = 0, mouseY = 0;
    float mouseDeltaX = 0, mouseDeltaY = 0;
    float scrollDelta = 0;
    bool leftButton = false, rightButton = false, middleButton = false;
    bool keys[256] = {};
};

class APlayerController : public AController {
public:
    APlayerController() { SetName("PlayerController"); }
    const char* GetClassName() const override { return "APlayerController"; }

    void OnMouseMove(float dx, float dy) {
        input.mouseDeltaX = dx;
        input.mouseDeltaY = dy;
    }

    void OnMouseButton(int button, bool pressed) {
        if (button == 0) input.leftButton = pressed;
        if (button == 1) input.rightButton = pressed;
        if (button == 2) input.middleButton = pressed;
    }

    void OnScroll(float delta) { input.scrollDelta = delta; }

    void OnKeyDown(int key) { if (key >= 0 && key < 256) input.keys[key] = true; }
    void OnKeyUp(int key) { if (key >= 0 && key < 256) input.keys[key] = false; }

    const InputState& GetInput() const { return input; }
    bool IsKeyDown(int key) const { return key >= 0 && key < 256 && input.keys[key]; }

    // Camera
    float cameraSpeed = 500.0f;
    float cameraZoomSpeed = 5.0f;
    float cameraMinZoom = 10.0f;
    float cameraMaxZoom = 500.0f;

    // Selection
    std::vector<int> selectedEntityIDs;

    void SelectEntity(int id) { selectedEntityIDs.clear(); selectedEntityIDs.push_back(id); }
    void AddToSelection(int id) { selectedEntityIDs.push_back(id); }
    void ClearSelection() { selectedEntityIDs.clear(); }
    bool IsSelected(int id) const {
        return std::find(selectedEntityIDs.begin(), selectedEntityIDs.end(), id) != selectedEntityIDs.end();
    }

    // Orders
    std::vector<std::string> pendingOrders;
    void QueueOrder(const std::string& order) { pendingOrders.push_back(order); }

protected:
    InputState input;
};
} // namespace ogb
