/* ArcLight Engine - Stellaris-Style In-Game UI
 * Developer: Stephen
 * Inspired by Paradox's Stellaris HUD: top bar, side panels, outliner,
 * galaxy map overlay, empire management, and contextual tooltips.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace arclight {

struct TopBarResource {
	std::string id;
	std::string name;
	std::string icon;
	float current = 0;
	float perMonth = 0;
	float capacity = 0;
	float3 color = float3(1, 1, 1);
};

class StellarisTopBar {
public:
	std::vector<TopBarResource> resources;
	bool isVisible = true;

	void Init() {
		AddResource("energy", "Energy Credits", 500.0f, 25.0f, 1000.0f, float3(1.0f, 0.9f, 0.0f));
		AddResource("minerals", "Minerals", 300.0f, 15.0f, 500.0f, float3(0.8f, 0.4f, 0.1f));
		AddResource("food", "Food", 200.0f, 10.0f, 200.0f, float3(0.2f, 0.8f, 0.2f));
		AddResource("alloys", "Alloys", 150.0f, 8.0f, 300.0f, float3(0.6f, 0.6f, 0.7f));
		AddResource("influence", "Influence", 50.0f, 2.0f, 100.0f, float3(0.3f, 0.3f, 1.0f));
		AddResource("unity", "Unity", 80.0f, 12.0f, 150.0f, float3(0.7f, 0.2f, 0.8f));
	}

	void AddResource(const std::string& id, const std::string& name, float cur, float per, float cap, const float3& col) {
		TopBarResource r; r.id = id; r.name = name; r.current = cur;
		r.perMonth = per; r.capacity = cap; r.color = col;
		resources.push_back(r);
	}

	void Update(float dt) {
		for (auto& r : resources) {
			r.current += r.perMonth * dt * 0.01f;
			if (r.capacity > 0) r.current = std::min(r.current, r.capacity);
			r.current = std::max(0.0f, r.current);
		}
	}
};

enum class OutlinerCategory { Empire, Planets, Fleets, Armies, Trade, Military };

struct OutlinerEntry {
	std::string id;
	std::string name;
	std::string icon;
	OutlinerCategory category = OutlinerCategory::Empire;
	int alertLevel = 0;
	bool isCollapsed = false;
	std::function<void()> onClick;
};

class StellarisOutliner {
public:
	std::vector<OutlinerEntry> entries;
	bool isVisible = true;
	bool isExpanded = true;
	float panelWidth = 280.0f;

	void AddEntry(const OutlinerEntry& e) { entries.push_back(e); }
	void ToggleCategory(OutlinerCategory cat) {
		for (auto& e : entries) if (e.category == cat) e.isCollapsed = !e.isCollapsed;
	}
};

enum class PanelType { None, PlanetView, FleetView, Diplomacy, Research, Economy, GalaxyMap };

class StellarisSidePanel {
public:
	PanelType currentPanel = PanelType::None;
	bool isVisible = false;
	float panelWidth = 450.0f;
	float animationProgress = 0.0f;
	std::string title;
	std::string subtitle;

	void OpenPanel(PanelType type, const std::string& t, const std::string& sub = "") {
		currentPanel = type; title = t; subtitle = sub; isVisible = true; animationProgress = 0.0f;
	}
	void ClosePanel() { currentPanel = PanelType::None; isVisible = false; animationProgress = 0.0f; }
	void Update(float dt) {
		if (isVisible && animationProgress < 1.0f) animationProgress = std::min(1.0f, animationProgress + dt * 4.0f);
		if (!isVisible && animationProgress > 0.0f) animationProgress = std::max(0.0f, animationProgress - dt * 6.0f);
	}
};

struct MapMarker {
	std::string id;
	std::string name;
	float3 position = ZeroVector;
	std::string type;
	int ownerTeam = -1;
	bool isSelected = false;
};

class StellarisGalaxyMap {
public:
	std::vector<MapMarker> markers;
	float zoom = 1.0f;
	float3 cameraPosition = ZeroVector;

	void AddMarker(const MapMarker& m) { markers.push_back(m); }
	void Zoom(float delta) { zoom = std::max(0.1f, std::min(10.0f, zoom + delta)); }
	void SelectMarker(const std::string& id) { for (auto& m : markers) m.isSelected = (m.id == id); }
};

struct TooltipData {
	std::string title;
	std::string description;
	std::vector<std::string> modifiers;
	bool hasWarning = false;
};

class StellarisTooltip {
public:
	TooltipData data;
	bool isVisible = false;
	float showDelay = 0.5f;
	float delayTimer = 0.0f;

	void Show(const TooltipData& td) { data = td; delayTimer = 0.0f; isVisible = false; }
	void Hide() { isVisible = false; delayTimer = 0.0f; }
	void Update(float dt) {
		if (!isVisible && delayTimer < showDelay) { delayTimer += dt; if (delayTimer >= showDelay) isVisible = true; }
	}
};

enum class NotificationType { Info, Warning, Critical, Diplomacy, Military, Economy };

struct Notification {
	std::string id;
	std::string title;
	std::string message;
	NotificationType type = NotificationType::Info;
	float timeRemaining = 5.0f;
};

class StellarisNotificationSystem {
public:
	std::vector<Notification> activeNotifications;
	int maxActive = 5;

	void AddNotification(const Notification& n) {
		activeNotifications.push_back(n);
		if (static_cast<int>(activeNotifications.size()) > maxActive)
			activeNotifications.erase(activeNotifications.begin());
	}
	void Update(float dt) {
		for (auto& n : activeNotifications) n.timeRemaining -= dt;
		activeNotifications.erase(
			std::remove_if(activeNotifications.begin(), activeNotifications.end(),
				[](const Notification& n) { return n.timeRemaining <= 0; }),
			activeNotifications.end());
	}
};

class StellarisUI {
public:
	StellarisTopBar topBar;
	StellarisOutliner outliner;
	StellarisSidePanel sidePanel;
	StellarisGalaxyMap galaxyMap;
	StellarisTooltip tooltip;
	StellarisNotificationSystem notifications;

	void Init() {
		topBar.Init();
		sidePanel.OpenPanel(PanelType::GalaxyMap, "Galaxy Map");
	}

	void Update(float dt) {
		topBar.Update(dt);
		sidePanel.Update(dt);
		tooltip.Update(dt);
		notifications.Update(dt);
	}

	void OpenPlanetView(const std::string& name) { sidePanel.OpenPanel(PanelType::PlanetView, name, "Planet Overview"); }
	void OpenFleetView(const std::string& name) { sidePanel.OpenPanel(PanelType::FleetView, name, "Fleet Management"); }
	void OpenDiplomacy(const std::string& name) { sidePanel.OpenPanel(PanelType::Diplomacy, name, "Diplomatic Relations"); }
};

} // namespace arclight
