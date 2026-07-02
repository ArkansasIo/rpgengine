/* ArcLight Engine - UE5-Style Editor UI
 * Developer: Stephen
 * Inspired by Unreal Engine 5's editor layout: viewport, details panel,
 * world outliner, content browser, toolbar, and command palette.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace arclight {

// ======================== Editor Viewport ========================

enum class ViewportMode {
	Perspective,
	Top,
	Front,
	Side,
	UV,
};

enum class GizmoType {
	Translate,
	Rotate,
	Scale,
	None,
};

struct EditorCamera {
	float3 position = float3(0, 100, -200);
	float3 rotation = float3(-20, 0, 0); // pitch, yaw, roll
	float moveSpeed = 500.0f;
	float rotationSpeed = 3.0f;
	float zoomSpeed = 50.0f;
	float panSpeed = 200.0f;
	float FOV = 90.0f;

	void MoveForward(float dt) {
		float3 forward = GetForward();
		position += forward * moveSpeed * dt;
	}

	void MoveRight(float dt) {
		float3 right = GetRight();
		position += right * moveSpeed * dt;
	}

	void MoveUp(float dt) {
		position.y += moveSpeed * dt;
	}

	void Rotate(float deltaYaw, float deltaPitch) {
		rotation.y += deltaYaw * rotationSpeed;
		rotation.x += deltaPitch * rotationSpeed;
		rotation.x = std::max(-89.0f, std::min(89.0f, rotation.x));
	}

	float3 GetForward() const {
		float yawRad = rotation.y * 3.14159f / 180.0f;
		float pitchRad = rotation.x * 3.14159f / 180.0f;
		return float3(-cosf(pitchRad) * sinf(yawRad), sinf(pitchRad), -cosf(pitchRad) * cosf(yawRad)).Normalize();
	}

	float3 GetRight() const {
		float yawRad = rotation.y * 3.14159f / 180.0f;
		return float3(cosf(yawRad), 0, -sinf(yawRad)).Normalize();
	}
};

class EditorViewport {
public:
	EditorCamera camera;
	ViewportMode mode = ViewportMode::Perspective;
	GizmoType activeGizmo = GizmoType::Translate;
	bool showGrid = true;
	bool showWireframe = false;
	bool showCollision = false;
	bool showNavigation = false;
	bool showPivotPoints = false;
	bool showBounds = false;
	bool showAxis = true;
	float gridSize = 100.0f;

	void Update(float /*dt*/) {
		// Camera update logic
	}

	void SetMode(ViewportMode m) { mode = m; }
	void SetGizmo(GizmoType g) { activeGizmo = g; }
	void ToggleGrid() { showGrid = !showGrid; }
	void ToggleWireframe() { showWireframe = !showWireframe; }
	void ToggleCollision() { showCollision = !showCollision; }
	void ResetCamera() {
		camera.position = float3(0, 100, -200);
		camera.rotation = float3(-20, 0, 0);
	}
};

// ======================== Details Panel (Properties) ========================

struct PropertyGroup {
	std::string name;
	bool isExpanded = true;
	std::vector<struct Property> properties;
};

struct Property {
	std::string name;
	std::string displayName;
	std::string type; // "float", "int", "bool", "string", "float3", "enum"
	std::string value;
	std::string category;
	bool isReadOnly = false;
	bool isDirty = false;
	float minValue = 0;
	float maxValue = 0;
	std::vector<std::string> enumOptions;
	std::function<void(const std::string&)> onValueChanged;
};

class EditorDetailsPanel {
public:
	std::string title = "Details";
	std::vector<PropertyGroup> groups;
	bool isVisible = true;
	float panelWidth = 350.0f;
	std::string searchFilter;

	void SetTitle(const std::string& t) { title = t; }

	void ClearGroups() { groups.clear(); }

	void AddGroup(const PropertyGroup& group) { groups.push_back(group); }

	void SetProperty(const std::string& groupName, const std::string& propName, const std::string& value) {
		for (auto& g : groups) {
			if (g.name == groupName) {
				for (auto& p : g.properties) {
					if (p.name == propName) {
						p.value = value;
						p.isDirty = true;
						if (p.onValueChanged) p.onValueChanged(value);
						return;
					}
				}
			}
		}
	}

	Property* FindProperty(const std::string& propName) {
		for (auto& g : groups) {
			for (auto& p : g.properties) {
				if (p.name == propName) return &p;
			}
		}
		return nullptr;
	}

	void ApplyDefaults() {
		for (auto& g : groups) {
			for (auto& p : g.properties) {
				p.isDirty = false;
			}
		}
	}
};

// ======================== World Outliner ========================

struct OutlinerObject {
	std::string id;
	std::string name;
	std::string typeName; // "Actor", "Light", "Camera", "StaticMesh", etc.
	bool isVisible = true;
	bool isLocked = false;
	bool isSelected = false;
	int depth = 0; // hierarchy depth
	std::string parentID;
	std::vector<std::string> childIDs;
};

class EditorWorldOutliner {
public:
	std::vector<OutlinerObject> objects;
	std::string searchFilter;
	bool isVisible = true;
	float panelWidth = 300.0f;

	void AddObject(const OutlinerObject& obj) { objects.push_back(obj); }

	void RemoveObject(const std::string& id) {
		objects.erase(
			std::remove_if(objects.begin(), objects.end(),
				[&](const OutlinerObject& o) { return o.id == id; }),
			objects.end()
		);
	}

	void SelectObject(const std::string& id) {
		for (auto& o : objects) o.isSelected = (o.id == id);
	}

	void DeselectAll() {
		for (auto& o : objects) o.isSelected = false;
	}

	std::vector<OutlinerObject*> GetSelected() {
		std::vector<OutlinerObject*> result;
		for (auto& o : objects) { if (o.isSelected) result.push_back(&o); }
		return result;
	}

	void ToggleVisibility(const std::string& id) {
		for (auto& o : objects) { if (o.id == id) { o.isVisible = !o.isVisible; return; } }
	}

	void ToggleLock(const std::string& id) {
		for (auto& o : objects) { if (o.id == id) { o.isLocked = !o.isLocked; return; } }
	}

	std::vector<OutlinerObject*> Search(const std::string& query) {
		std::vector<OutlinerObject*> result;
		for (auto& o : objects) {
			if (o.name.find(query) != std::string::npos) result.push_back(&o);
		}
		return result;
	}
};

// ======================== Content Browser ========================

struct ContentAsset {
	std::string path;
	std::string name;
	std::string type; // "Texture", "Mesh", "Material", "Blueprint", "Sound", "Script"
	size_t fileSize = 0;
	std::string lastModified;
};

class EditorContentBrowser {
public:
	std::vector<ContentAsset> assets;
	std::string currentPath = "/";
	std::string searchFilter;
	bool isVisible = true;
	float panelHeight = 300.0f;
	bool showThumbnails = true;
	int thumbnailSize = 64;

	void NavigateTo(const std::string& path) { currentPath = path; }
	void GoUp() {
		size_t pos = currentPath.rfind('/', currentPath.size() - 2);
		if (pos != std::string::npos) currentPath = currentPath.substr(0, pos + 1);
		else currentPath = "/";
	}

	void AddAsset(const ContentAsset& asset) { assets.push_back(asset); }

	std::vector<ContentAsset> GetAssetsInPath(const std::string& path) const {
		std::vector<ContentAsset> result;
		for (auto& a : assets) {
			if (a.path.find(path) == 0 && a.path != path) result.push_back(a);
		}
		return result;
	}

	std::vector<ContentAsset> Search(const std::string& query) const {
		std::vector<ContentAsset> result;
		for (auto& a : assets) {
			if (a.name.find(query) != std::string::npos) result.push_back(a);
		}
		return result;
	}
};

// ======================== Toolbar ========================

struct ToolbarButton {
	std::string id;
	std::string label;
	std::string icon;
	std::string tooltip;
	bool isEnabled = true;
	bool isActive = false;
	std::function<void()> onClick;
};

class EditorToolbar {
public:
	std::vector<ToolbarButton> buttons;
	bool isVisible = true;
	float toolbarHeight = 40.0f;

	void Init() {
		AddButton("save", "Save", "Save the current level", []() {});
		AddButton("save_all", "Save All", "Save all modified assets", []() {});
		AddButton("undo", "Undo", "Undo last action", []() {});
		AddButton("redo", "Redo", "Redo last action", []() {});
		AddButton("play", "Play", "Play in editor", []() {});
		AddButton("pause", "Pause", "Pause simulation", []() {});
		AddButton("stop", "Stop", "Stop simulation", []() {});
	}

	void AddButton(const std::string& id, const std::string& label, const std::string& tooltip, std::function<void()> cb) {
		ToolbarButton btn;
		btn.id = id; btn.label = label; btn.tooltip = tooltip; btn.onClick = std::move(cb);
		buttons.push_back(btn);
	}

	void OnClick(const std::string& id) {
		for (auto& btn : buttons) {
			if (btn.id == id && btn.isEnabled && btn.onClick) btn.onClick();
		}
	}
};

// ======================== Command Palette ========================

struct EditorCommand {
	std::string id;
	std::string name;
	std::string category;
	std::string shortcut;
	bool isEnabled = true;
	std::function<void()> execute;
};

class EditorCommandPalette {
public:
	std::vector<EditorCommand> commands;
	bool isOpen = false;
	std::string searchQuery;
	int selectedindex = 0;

	void RegisterCommand(const EditorCommand& cmd) { commands.push_back(cmd); }

	void Toggle() { isOpen = !isOpen; searchQuery.clear(); selectedindex = 0; }

	void Search(const std::string& query) {
		searchQuery = query;
		selectedindex = 0;
	}

	std::vector<EditorCommand*> GetFilteredCommands() {
		std::vector<EditorCommand*> result;
		for (auto& cmd : commands) {
			if (searchQuery.empty() || cmd.name.find(searchQuery) != std::string::npos) {
				result.push_back(&cmd);
			}
		}
		return result;
	}

	void ExecuteSelected() {
		auto filtered = GetFilteredCommands();
		if (selectedindex >= 0 && selectedindex < static_cast<int>(filtered.size())) {
			if (filtered[selectedindex]->isEnabled && filtered[selectedindex]->execute)
				filtered[selectedindex]->execute();
		}
	}

	void NavigateUp() { if (selectedindex > 0) selectedindex--; }
	void NavigateDown() { selectedindex++; }
};

// ======================== Main Editor UI Manager ========================

class UE5EditorUI {
public:
	EditorViewport viewport;
	EditorDetailsPanel detailsPanel;
	EditorWorldOutliner worldOutliner;
	EditorContentBrowser contentBrowser;
	EditorToolbar toolbar;
	EditorCommandPalette commandPalette;

	bool isInitialized = false;
	float uiScale = 1.0f;

	void Init() {
		toolbar.Init();
		isInitialized = true;
	}

	void Update(float dt) {
		viewport.Update(dt);
	}

	void ToggleCommandPalette() { commandPalette.Toggle(); }

	void SelectObject(const std::string& id) {
		worldOutliner.SelectObject(id);
		detailsPanel.ClearGroups();
		PropertyGroup transform;
		transform.name = "Transform";
		transform.properties.push_back({"position", "Location", "float3", "0, 0, 0"});
		transform.properties.push_back({"rotation", "Rotation", "float3", "0, 0, 0"});
		transform.properties.push_back({"scale", "Scale", "float3", "1, 1, 1"});
		detailsPanel.AddGroup(transform);
	}
};

} // namespace arclight
