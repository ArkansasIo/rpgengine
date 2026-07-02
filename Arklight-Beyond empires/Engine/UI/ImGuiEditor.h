/* ArkLight Beyond - Dear ImGui Editor
 * Developer: Stephen
 * Full editor interface with panels, inspectors, and tools.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace ogb {

struct ImGuiPanel {
    std::string name;
    bool isVisible = true;
    bool isFocused = false;
    float x = 0, y = 0, w = 400, h = 300;
    std::function<void()> renderFunc;
};

struct PropertyField {
    std::string name;
    std::string type; // "float", "int", "bool", "string", "float3", "enum"
    void* value = nullptr;
    float minVal = 0, maxVal = 1;
    std::vector<std::string> enumOptions;
    std::function<void()> onValueChanged;
};

struct InspectorSection {
    std::string name;
    bool isExpanded = true;
    std::vector<PropertyField> properties;
};

class ImGuiEditor {
public:
    // Panels
    ImGuiPanel worldOutliner;
    ImGuiPanel detailsPanel;
    ImGuiPanel contentBrowser;
    ImGuiPanel outputLog;
    ImGuiPanel placeActors;
    ImGuiPanel menuBar;
    ImGuiPanel toolbar;

    // State
    std::string selectedActorName;
    std::string selectedActorType;
    std::vector<InspectorSection> inspectorSections;
    std::vector<std::string> logEntries;
    bool showDemoWindow = false;
    float uiScale = 1.0f;

    // Content Browser
    struct ContentItem {
        std::string name;
        std::string path;
        std::string type; // "Blueprint", "Texture", "Mesh", "Material", "Script"
        std::string icon;
        size_t fileSize = 0;
    };
    struct ContentFolder {
        std::string name;
        std::vector<ContentFolder> subfolders;
        std::vector<ContentItem> items;
        bool isExpanded = true;
    };
    ContentFolder rootFolder;
    std::string currentPath = "/";
    std::string searchFilter;

    // World Outliner
    struct OutlinerEntry {
        std::string id;
        std::string name;
        std::string typeName;
        std::string icon;
        bool isVisible = true;
        bool isLocked = false;
        bool isSelected = false;
    };
    std::vector<OutlinerEntry> outlinerEntries;

    void Init() {
        worldOutliner = {"World Outliner", true, false, 0, 0, 280, 600, nullptr};
        detailsPanel = {"Details", true, false, 280, 0, 350, 600, nullptr};
        contentBrowser = {"Content Browser", true, false, 280, 400, 700, 300, nullptr};
        outputLog = {"Output Log", true, false, 280, 700, 1000, 200, nullptr};
        placeActors = {"Place Actors", true, false, 980, 0, 300, 600, nullptr};
        toolbar = {"Toolbar", true, false, 0, 0, 1280, 40, nullptr};
        menuBar = {"Menu", true, false, 0, 0, 1280, 25, nullptr};
        SetupContentBrowser();
        SetupOutliner();
    }

    void SelectActor(const std::string& id) {
        for (auto& e : outlinerEntries) e.isSelected = (e.id == id);
        for (auto& s : inspectorSections) s.isExpanded = true;
    }

    void LogMessage(const std::string& msg) {
        logEntries.push_back(msg);
        if (static_cast<int>(logEntries.size()) > 1000) logEntries.erase(logEntries.begin());
    }

    void AddInspectorSection(const std::string& name, const std::vector<PropertyField>& fields) {
        inspectorSections.push_back({name, true, fields});
    }

    void ClearInspector() { inspectorSections.clear(); }

private:
    void SetupContentBrowser() {
        rootFolder.name = "Content";
        ContentFolder blueprints; blueprints.name = "Blueprints";
        ContentFolder buildings; buildings.name = "Buildings";
        buildings.items = {
            {"BP_CommandCenter", "/Blueprints/Buildings/BP_CommandCenter", "BlueprintClass", "🏰"},
            {"BP_PowerPlant", "/Blueprints/Buildings/BP_PowerPlant", "BlueprintClass", "⚡"},
            {"BP_Shipyard", "/Blueprints/Buildings/BP_Shipyard", "BlueprintClass", "🏗"},
            {"BP_Barracks", "/Blueprints/Buildings/BP_Barracks", "BlueprintClass", "🏠"},
            {"BP_ResearchLab", "/Blueprints/Buildings/BP_ResearchLab", "BlueprintClass", "🔬"},
            {"BP_MissileSilo", "/Blueprints/Buildings/BP_MissileSilo", "BlueprintClass", "🚀"},
        };
        ContentFolder units; units.name = "Units";
        units.items = {
            {"BP_Fighter", "/Blueprints/Units/BP_Fighter", "BlueprintClass", "✈"},
            {"BP_Cruiser", "/Blueprints/Units/BP_Cruiser", "BlueprintClass", "🔲"},
            {"BP_ColonyShip", "/Blueprints/Units/BP_ColonyShip", "BlueprintClass", "🏠"},
        };
        buildings.subfolders.push_back(units);
        blueprints.subfolders.push_back(buildings);
        ContentFolder models; models.name = "Models";
        ContentFolder textures; textures.name = "Textures";
        ContentFolder materials; materials.name = "Materials";
        rootFolder.subfolders = {blueprints, models, textures, materials};
    }

    void SetupOutliner() {
        outlinerEntries = {
            {"dir_light", "DirectionalLight", "Light", "☀", true, false, false},
            {"sky_atmo", "SkyAtmosphere", "Atmosphere", "🌤", true, false, false},
            {"landscape", "Landscape", "Terrain", "🏔", true, false, false},
            {"water", "Water", "Water", "🌊", true, false, false},
            {"player_start", "PlayerStart", "Player", "📍", true, false, false},
            {"empire_mgr", "EmpireManager", "Manager", "👑", true, false, false},
            {"fleet_mgr", "FleetManager", "Manager", "🚀", true, false, false},
        };
    }
};

} // namespace ogb
