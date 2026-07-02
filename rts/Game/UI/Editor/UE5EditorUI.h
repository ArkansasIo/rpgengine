/* ArcLight Engine - UE5-Style Editor UI
 * Developer: Stephen
 * Inspired by Unreal Engine 5 editor: World Outliner, Details Panel,
 * Place Actors, Content Browser, Output Log, Toolbar, Menu Bar.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace arclight {

// ======================== Menu Bar ========================

struct MenuItemDef {
	std::string label;
	std::string shortcut;
	bool isEnabled = true;
	bool isSeparator = false;
	std::function<void()> onClick;
	std::vector<MenuItemDef> subItems;
};

class EditorMenuBar {
public:
	std::vector<MenuItemDef> menus;

	void Init() {
		MenuItemDef fileMenu;
		fileMenu.label = "File";
		fileMenu.subItems = {
			{"New Level", "Ctrl+N", true, false, [](){}},
			{"Open Level", "Ctrl+O", true, false, [](){}},
			{"Save Current Level", "Ctrl+S", true, false, [](){}},
			{"Save All", "Ctrl+Shift+S", true, false, [](){}},
			{"", "", false, true, nullptr},
			{"Import", "", true, false, [](){}},
			{"Export", "", true, false, [](){}},
			{"", "", false, true, nullptr},
			{"Exit", "", true, false, [](){}},
		};

		MenuItemDef editMenu;
		editMenu.label = "Edit";
		editMenu.subItems = {
			{"Undo", "Ctrl+Z", true, false, [](){}},
			{"Redo", "Ctrl+Shift+Z", true, false, [](){}},
			{"", "", false, true, nullptr},
			{"Select All", "Ctrl+A", true, false, [](){}},
			{"Duplicate", "Ctrl+D", true, false, [](){}},
			{"Delete", "Delete", true, false, [](){}},
		};

		MenuItemDef windowMenu;
		windowMenu.label = "Window";
		windowMenu.subItems = {
			{"World Outliner", "", true, false, [](){}},
			{"Details Panel", "", true, false, [](){}},
			{"Content Browser", "", true, false, [](){}},
			{"Output Log", "", true, false, [](){}},
			{"Place Actors", "", true, false, [](){}},
		};

		MenuItemDef toolsMenu;
		toolsMenu.label = "Tools";
		toolsMenu.subItems = {
			{"Refresh Content Browser", "", true, false, [](){}},
			{"Regenerate Project Files", "", true, false, [](){}},
		};

		MenuItemDef buildMenu;
		buildMenu.label = "Build";
		buildMenu.subItems = {
			{"Build Selected Level", "Ctrl+Shift+B", true, false, [](){}},
			{"Build All Levels", "Ctrl+Shift+Alt+B", true, false, [](){}},
			{"", "", false, true, nullptr},
			{"Refresh Navigable Geometry", "", true, false, [](){}},
		};

		MenuItemDef selectMenu;
		selectMenu.label = "Select";
		selectMenu.subItems = {
			{"Select All", "Ctrl+A", true, false, [](){}},
			{"Deselect All", "Esc", true, false, [](){}},
			{"Select Inverse", "Ctrl+Shift+A", true, false, [](){}},
			{"Select by Type", "", true, false, [](){}},
		};

		MenuItemDef actorMenu;
		actorMenu.label = "Actor";
		actorMenu.subItems = {
			{"Add Actor", "", true, false, [](){}},
			{"Duplicate", "Ctrl+D", true, false, [](){}},
			{"Delete", "Delete", true, false, [](){}},
			{"", "", false, true, nullptr},
			{"Lock Actor", "Alt+L", true, false, [](){}},
			{"Hide Actor", "H", true, false, [](){}},
		};

		MenuItemDef helpMenu;
		helpMenu.label = "Help";
		helpMenu.subItems = {
			{"Documentation", "", true, false, [](){}},
			{"API Reference", "", true, false, [](){}},
			{"", "", false, true, nullptr},
			{"About ArcLight Engine", "", true, false, [](){}},
		};

		menus = {fileMenu, editMenu, windowMenu, toolsMenu, buildMenu, selectMenu, actorMenu, helpMenu};
	}
};

// ======================== Toolbar ========================

enum class EditorMode {
	Selection,
	Placement,
	Geometry,
	Paint,
	Landscape,
	Foliage,
};

struct ToolbarButtonDef {
	std::string id;
	std::string label;
	std::string tooltip;
	std::string icon;
	bool isEnabled = true;
	bool isActive = false;
	std::function<void()> onClick;
};

class EditorToolbar {
public:
	EditorMode currentMode = EditorMode::Selection;
	std::vector<ToolbarButtonDef> buttons;
	bool showPlayControls = true;

	void Init() {
		AddButton("select", "Selection Mode", "Select and manipulate actors", "S", true, nullptr);
		AddButton("place", "Placement Mode", "Place new actors in the world", "P", true, nullptr);
		AddButton("geometry", "Geometry Mode", "Edit geometry", "G", true, nullptr);
		AddButton("paint", "Paint Mode", "Paint vertex colors or weights", "T", true, nullptr);

		AddButton("play", "Play", "Play in editor", "▶", true, nullptr);
		AddButton("pause", "Pause", "Pause simulation", "⏸", true, nullptr);
		AddButton("stop", "Stop", "Stop simulation", "⏹", true, nullptr);
		AddButton("simulate", "Simulate", "Simulate without playing", "⚡", true, nullptr);
	}

	void AddButton(const std::string& id, const std::string& label, const std::string& tooltip, const std::string& icon, bool enabled, std::function<void()> cb) {
		ToolbarButtonDef btn;
		btn.id = id; btn.label = label; btn.tooltip = tooltip; btn.icon = icon;
		btn.isEnabled = enabled; btn.onClick = std::move(cb);
		buttons.push_back(btn);
	}

	void SetMode(EditorMode mode) { currentMode = mode; }
};

// ======================== World Outliner ========================

struct OutlinerActor {
	std::string id;
	std::string name;
	std::string typeName;
	std::string icon;
	bool isVisible = true;
	bool isLocked = false;
	bool isSelected = false;
	int depth = 0;
	std::string parentID;
	std::vector<std::string> childIDs;
};

struct EditorOutlinerCategory {
	std::string name;
	std::vector<OutlinerActor> actors;
	bool isExpanded = true;
};

class EditorWorldOutliner {
public:
	std::string title = "World Outliner";
	std::vector<EditorOutlinerCategory> categories;
	std::string searchFilter;
	bool isVisible = true;
	float panelWidth = 260.0f;
	int totalActors = 0;
	int selectedCount = 0;

	void Init() {
		// Map category
		EditorOutlinerCategory map;
		map.name = "Map";
		map.actors = {
			{"dir_light", "DirectionalLight", "Light", "☀", true, false, false},
			{"exponential_fog", "ExponentialHeightFog", "Fog", "🌫", true, false, false},
			{"sky_atmosphere", "SkyAtmosphere", "Atmosphere", "🌤", true, false, false},
			{"sky_light", "SkyLight", "Light", "💡", true, false, false},
			{"sky_sphere", "SM_SkySphere", "StaticMesh", "🌐", true, false, false},
			{"volumetric_cloud", "VolumetricCloud", "Cloud", "☁", true, false, false},
		};

		// Terrain category
		EditorOutlinerCategory terrain;
		terrain.name = "Terrain";
		terrain.actors = {
			{"landscape", "Landscape", "Terrain", "🏔", true, false, false},
			{"water", "Water", "Water", "🌊", true, false, false},
		};

		// Gameplay category
		EditorOutlinerCategory gameplay;
		gameplay.name = "Gameplay";
		gameplay.actors = {
			{"player_start", "PlayerStart", "PlayerStart", "📍", true, false, false},
			{"resource_mgr", "ResourceManager", "Manager", "📊", true, false, false},
			{"ai_manager", "AI_Manager", "Manager", "🤖", true, false, false},
		};

		// Factions category
		EditorOutlinerCategory factions;
		factions.name = "Factions";
		factions.actors = {
			{"faction_nova", "Faction_NovaPrime", "Faction", "🔵", true, false, false},
			{"faction_zenith", "Faction_Zenith", "Faction", "🟣", true, false, false},
			{"faction_aeon", "Faction_Aeon", "Faction", "🟢", true, false, false},
		};

		categories.push_back(map);
		categories.push_back(terrain);
		categories.push_back(gameplay);
		categories.push_back(factions);
		UpdateCounts();
	}

	void SelectActor(const std::string& id) {
		for (auto& cat : categories) {
			for (auto& actor : cat.actors) {
				if (actor.id == id) { actor.isSelected = true; selectedCount++; }
				else { actor.isSelected = false; }
			}
		}
	}

	void DeselectAll() {
		for (auto& cat : categories) {
			for (auto& actor : cat.actors) actor.isSelected = false;
		}
		selectedCount = 0;
	}

	void ToggleVisibility(const std::string& id) {
		for (auto& cat : categories) {
			for (auto& actor : cat.actors) {
				if (actor.id == id) { actor.isVisible = !actor.isVisible; return; }
			}
		}
	}

	void ToggleCategory(const std::string& name) {
		for (auto& cat : categories) {
			if (cat.name == name) { cat.isExpanded = !cat.isExpanded; return; }
		}
	}

	void UpdateCounts() {
		totalActors = 0;
		for (auto& cat : categories) totalActors += static_cast<int>(cat.actors.size());
	}

	std::vector<OutlinerActor*> Search(const std::string& query) {
		std::vector<OutlinerActor*> result;
		for (auto& cat : categories) {
			for (auto& actor : cat.actors) {
				if (query.empty() || actor.name.find(query) != std::string::npos) {
					result.push_back(&actor);
				}
			}
		}
		return result;
	}
};

// ======================== Details Panel ========================

struct DetailProperty {
	std::string name;
	std::string displayName;
	std::string type;
	std::string value;
	bool isReadOnly = false;
	bool isExpanded = true;
	float minValue = 0;
	float maxValue = 0;
	std::function<void(const std::string&)> onValueChanged;
};

struct DetailSection {
	std::string name;
	bool isExpanded = true;
	std::vector<DetailProperty> properties;
};

class EditorDetailsPanel {
public:
	std::string title = "Details";
	std::string selectedActorName;
	std::string selectedActorType;
	std::vector<DetailSection> sections;
	bool isVisible = true;
	float panelWidth = 320.0f;
	std::string searchFilter;

	void ShowActor(const std::string& name, const std::string& type) {
		selectedActorName = name;
		selectedActorType = type;
		sections.clear();

		// Transform section
		DetailSection transform;
		transform.name = "Transform";
		transform.properties = {
			{"locationX", "Location X", "float", "12346.0"},
			{"locationY", "Location Y", "float", "6785.0"},
			{"locationZ", "Location Z", "float", "210.0"},
			{"rotationX", "Rotation X", "float", "0.0"},
			{"rotationY", "Rotation Y", "float", "0.0"},
			{"rotationZ", "Rotation Z", "float", "0.0"},
			{"scaleX", "Scale X", "float", "1.0"},
			{"scaleY", "Scale Y", "float", "1.0"},
			{"scaleZ", "Scale Z", "float", "1.0"},
		};

		// General section
		DetailSection general;
		general.name = "General";
		general.properties = {
			{"health", "Health", "float", "10000.0"},
			{"shield", "Shield", "float", "5000.0"},
			{"faction", "Faction", "enum", "NovaPrime"},
			{"buildTime", "Build Time", "float", "120.0"},
			{"energyConsume", "Energy Consume", "float", "250.0"},
			{"uiIcon", "UI Icon", "texture", "T_CommandCenter_Icon"},
		};

		sections = {transform, general};
	}

	void SetProperty(const std::string& section, const std::string& prop, const std::string& value) {
		for (auto& sec : sections) {
			if (sec.name == section) {
				for (auto& p : sec.properties) {
					if (p.name == prop) { p.value = value; if (p.onValueChanged) p.onValueChanged(value); return; }
				}
			}
		}
	}
};

// ======================== Place Actors ========================

struct PlaceableActorDef {
	std::string id;
	std::string name;
	std::string category;
	std::string subcategory;
	std::string icon;
	std::string description;
};

struct PlaceActorCategory {
	std::string name;
	std::vector<PlaceableActorDef> actors;
	bool isExpanded = true;
};

class EditorPlaceActors {
public:
	std::string title = "Place Actors";
	std::vector<PlaceActorCategory> categories;
	std::string searchFilter;
	bool isVisible = true;
	float panelWidth = 260.0f;

	void Init() {
		PlaceActorCategory basic;
		basic.name = "Basic";
		basic.actors = {
			{"empty_actor", "Empty Actor", "Basic", "Actor", "⚪", "An empty actor with no components"},
			{"empty_character", "Empty Character", "Basic", "Character", "🧑", "An empty character with movement"},
			{"empty_pawn", "Empty Pawn", "Basic", "Pawn", "♟", "An empty controllable pawn"},
			{"point_light", "Point Light", "Basic", "Light", "💡", "Emits light in all directions"},
			{"player_start", "Player Start", "Basic", "Player", "📍", "Spawn point for players"},
			{"trigger_box", "Trigger Box", "Basic", "Trigger", "📦", "Box-shaped trigger volume"},
			{"trigger_sphere", "Trigger Sphere", "Basic", "Trigger", "⭕", "Sphere-shaped trigger volume"},
		};

		PlaceActorCategory lights;
		lights.name = "Lights";
		lights.isExpanded = false;
		lights.actors = {
			{"point_light2", "Point Light", "Lights", "Light", "💡", "Point light source"},
			{"spot_light", "Spot Light", "Lights", "Light", "🔦", "Cone-shaped light"},
			{"rect_light", "Rect Light", "Lights", "Light", "🔲", "Rectangular area light"},
			{"dir_light", "Directional Light", "Lights", "Light", "☀", "Infinite directional light"},
			{"sky_light", "Sky Light", "Lights", "Light", "🌙", "Captures distant environment light"},
		};

		PlaceActorCategory geometry;
		geometry.name = "Geometry";
		geometry.isExpanded = false;
		geometry.actors = {
			{"cube", "Cube", "Geometry", "Shape", "⬜", "Static mesh cube"},
			{"sphere", "Sphere", "Geometry", "Shape", "⚪", "Static mesh sphere"},
			{"cylinder", "Cylinder", "Geometry", "Shape", "🔵", "Static mesh cylinder"},
			{"plane", "Plane", "Geometry", "Shape", "▬", "Static mesh plane"},
			{"cone", "Cone", "Geometry", "Shape", "🔺", "Static mesh cone"},
		};

		categories = {basic, lights, geometry};
	}

	std::vector<PlaceableActorDef*> Search(const std::string& query) {
		std::vector<PlaceableActorDef*> result;
		for (auto& cat : categories) {
			for (auto& actor : cat.actors) {
				if (query.empty() || actor.name.find(query) != std::string::npos) {
					result.push_back(&actor);
				}
			}
		}
		return result;
	}
};

// ======================== Content Browser ========================

struct ContentAssetDef {
	std::string name;
	std::string path;
	std::string type;
	std::string subType;
	std::string thumbnail;
	std::string description;
	size_t fileSize = 0;
};

struct ContentFolder {
	std::string name;
	std::string path;
	std::vector<ContentFolder> subFolders;
	std::vector<ContentAssetDef> assets;
	bool isExpanded = true;
};

class EditorContentBrowser {
public:
	std::string title = "Content Browser";
	std::vector<ContentFolder> rootFolders;
	std::string currentPath;
	std::string searchFilter;
	bool isVisible = true;
	float panelHeight = 200.0f;
	bool showThumbnails = true;
	bool showDetails = false;
	int selectedAssetCount = 0;

	void Init() {
		ContentFolder root;
		root.name = "OGAME_CONTENT";
		root.path = "/";

		// Blueprints
		ContentFolder blueprints;
		blueprints.name = "Blueprints";
		blueprints.path = "/Blueprints/";

		ContentFolder buildings;
		buildings.name = "Buildings";
		buildings.path = "/Blueprints/Buildings/";
		buildings.assets = {
			{"BP_CommandCenter", "/Blueprints/Buildings/BP_CommandCenter", "BlueprintClass", "Building", "🏰", "Main command center building"},
			{"BP_PowerPlant", "/Blueprints/Buildings/BP_PowerPlant", "BlueprintClass", "Building", "⚡", "Generates energy for the base"},
			{"BP_MetalExtractor", "/Blueprints/Buildings/BP_MetalExtractor", "BlueprintClass", "Building", "⛏", "Extracts metal resources"},
			{"BP_CrystalExtractor", "/Blueprints/Buildings/BP_CrystalExtractor", "BlueprintClass", "Building", "💎", "Extracts crystal resources"},
			{"BP_DeuteriumSynthesizer", "/Blueprints/Buildings/BP_DeuteriumSynthesizer", "BlueprintClass", "Building", "🧪", "Produces deuterium fuel"},
			{"BP_SolarPlant", "/Blueprints/Buildings/BP_SolarPlant", "BlueprintClass", "Building", "☀", "Solar energy collector"},
			{"BP_FusionReactor", "/Blueprints/Buildings/BP_FusionReactor", "BlueprintClass", "Building", "🔬", "High-output fusion reactor"},
			{"BP_Shipyard", "/Blueprints/Buildings/BP_Shipyard", "BlueprintClass", "Building", "🏗", "Constructs ships and vehicles"},
			{"BP_Barracks", "/Blueprints/Buildings/BP_Barracks", "BlueprintClass", "Building", "🏠", "Trains infantry units"},
			{"BP_ResearchLab", "/Blueprints/Buildings/BP_ResearchLab", "BlueprintClass", "Building", "🔬", "Unlocks new technologies"},
			{"BP_ShieldGenerator", "/Blueprints/Buildings/BP_ShieldGenerator", "BlueprintClass", "Building", "🛡", "Generates protective shields"},
			{"BP_MissileTurret", "/Blueprints/Buildings/BP_MissileTurret", "BlueprintClass", "Building", "🚀", "Automated defense turret"},
		};

		ContentFolder units;
		units.name = "Units";
		units.path = "/Blueprints/Units/";
		units.isExpanded = false;
		units.assets = {
			{"BP_Commander", "/Blueprints/Units/BP_Commander", "BlueprintClass", "Unit", "⭐", "Hero commander unit"},
			{"BP_Tank", "/Blueprints/Units/BP_Tank", "BlueprintClass", "Unit", "🔲", "Main battle tank"},
			{"BP_Scout", "/Blueprints/Units/BP_Scout", "BlueprintClass", "Unit", "🔍", "Fast reconnaissance unit"},
			{"BP_Bomber", "/Blueprints/Units/BP_Bomber", "BlueprintClass", "Unit", "💣", "Air-to-ground bomber"},
			{"BP_Fighter", "/Blueprints/Units/BP_Fighter", "BlueprintClass", "Unit", "✈", "Air superiority fighter"},
		};

		buildings.subFolders = {units};
		blueprints.subFolders = {buildings};
		root.subFolders = {blueprints};

		rootFolders = {root};
	}

	void NavigateTo(const std::string& path) { currentPath = path; }
	void GoUp() {
		auto pos = currentPath.rfind('/', currentPath.size() - 2);
		if (pos != std::string::npos) currentPath = currentPath.substr(0, pos + 1);
		else currentPath = "/";
	}
};

// ======================== Output Log ========================

enum class LogVerbosity {
	Display,
	Warning,
	Error,
	Fatal,
};

struct LogEntry {
	std::string message;
	LogVerbosity verbosity = LogVerbosity::Display;
	std::string timestamp;
};

class EditorOutputLog {
public:
	std::string title = "Output Log";
	std::vector<LogEntry> entries;
	bool isVisible = true;
	float panelHeight = 150.0f;
	LogVerbosity filterVerbosity = LogVerbosity::Display;
	bool autoScroll = true;
	int maxEntries = 1000;

	void Log(const std::string& msg, LogVerbosity verbosity = LogVerbosity::Display) {
		LogEntry entry;
		entry.message = msg;
		entry.verbosity = verbosity;
		entries.push_back(entry);
		if (static_cast<int>(entries.size()) > maxEntries) {
			entries.erase(entries.begin());
		}
	}

	void Clear() { entries.clear(); }

	std::vector<LogEntry> GetFiltered() const {
		std::vector<LogEntry> result;
		for (auto& e : entries) {
			if (e.verbosity >= filterVerbosity) result.push_back(e);
		}
		return result;
	}
};

// ======================== Viewport ========================

enum class ViewportMode { Perspective, Top, Front, Side, UV };
enum class GizmoType { Translate, Rotate, Scale, None };

struct EditorCamera {
	float3 position = float3(0, 100, -200);
	float3 rotation = float3(-20, 0, 0);
	float moveSpeed = 500.0f;
	float rotationSpeed = 3.0f;

	void MoveForward(float dt) {
		float yawRad = rotation.y * 3.14159f / 180.0f;
		float pitchRad = rotation.x * 3.14159f / 180.0f;
		float3 forward(-cosf(pitchRad) * sinf(yawRad), sinf(pitchRad), -cosf(pitchRad) * cosf(yawRad));
		position += forward * moveSpeed * dt;
	}

	void Rotate(float deltaYaw, float deltaPitch) {
		rotation.y += deltaYaw * rotationSpeed;
		rotation.x = std::max(-89.0f, std::min(89.0f, rotation.x + deltaPitch * rotationSpeed));
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
	float FOV = 90.0f;
	bool isLit = true;
	bool showOverlay = true;

	void SetMode(ViewportMode m) { mode = m; }
	void SetGizmo(GizmoType g) { activeGizmo = g; }
	void ToggleGrid() { showGrid = !showGrid; }
	void ToggleWireframe() { showWireframe = !showWireframe; }
	void ResetCamera() {
		camera.position = float3(0, 100, -200);
		camera.rotation = float3(-20, 0, 0);
	}
};

// ======================== Main Editor UI ========================

class UE5EditorUI {
public:
	EditorMenuBar menuBar;
	EditorToolbar toolbar;
	EditorWorldOutliner worldOutliner;
	EditorDetailsPanel detailsPanel;
	EditorPlaceActors placeActors;
	EditorContentBrowser contentBrowser;
	EditorOutputLog outputLog;
	EditorViewport viewport;

	bool isInitialized = false;
	float uiScale = 1.0f;
	std::string windowTitle = "OGAME_BEYOND - ArcLight Engine Editor";
	bool isPlaying = false;
	bool isPaused = false;

	void Init() {
		menuBar.Init();
		toolbar.Init();
		worldOutliner.Init();
		placeActors.Init();
		contentBrowser.Init();
		outputLog.Log("ArcLight Engine Editor initialized", LogVerbosity::Display);
		outputLog.Log("World loaded: OGAME_WORLD", LogVerbosity::Display);
		outputLog.Log("715 actors in scene", LogVerbosity::Display);
		outputLog.Log("Build successful: 0 errors, 0 warnings", LogVerbosity::Display);
		isInitialized = true;
	}

	void Update(float dt) {
		viewport.camera.MoveForward(dt);
	}

	void SelectActor(const std::string& id) {
		worldOutliner.SelectActor(id);
		for (auto& cat : worldOutliner.categories) {
			for (auto& actor : cat.actors) {
				if (actor.id == id) {
					detailsPanel.ShowActor(actor.name, actor.typeName);
					outputLog.Log("Selected: " + actor.name, LogVerbosity::Display);
					return;
				}
			}
		}
	}

	void Play() {
		isPlaying = true;
		isPaused = false;
		outputLog.Log("Play in Editor started", LogVerbosity::Display);
	}

	void Pause() {
		isPaused = !isPaused;
		outputLog.Log(isPaused ? "Simulation paused" : "Simulation resumed", LogVerbosity::Display);
	}

	void Stop() {
		isPlaying = false;
		isPaused = false;
		outputLog.Log("Play in Editor stopped", LogVerbosity::Display);
	}

	void ToggleCommandPalette() {}
};

} // namespace arclight
