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

// ======================== Top Bar (Empire Resources) ========================

struct TopBarResource {
	std::string id;
	std::string name;
	std::string icon; // icon identifier
	float current = 0;
	float perMonth = 0;
	float capacity = 0;
	bool showPlusSign = true;
	float3 color = float3(1, 1, 1);
};

class StellarisTopBar {
public:
	std::vector<TopBarResource> resources;
	bool isVisible = true;

	void Init() {
		resources.clear();
		AddResource("energy", "Energy Credits", 500.0f, 25.0f, 1000.0f, float3(1.0f, 0.9f, 0.0f));
		AddResource("minerals", "Minerals", 300.0f, 15.0f, 500.0f, float3(0.8f, 0.4f, 0.1f));
		AddResource("food", "Food", 200.0f, 10.0f, 200.0f, float3(0.2f, 0.8f, 0.2f));
		AddResource("alloys", "Alloys", 150.0f, 8.0f, 300.0f, float3(0.6f, 0.6f, 0.7f));
		AddResource("consumer_goods", "Consumer Goods", 100.0f, 5.0f, 200.0f, float3(0.9f, 0.9f, 0.9f));
		AddResource("influence", "Influence", 50.0f, 2.0f, 100.0f, float3(0.3f, 0.3f, 1.0f));
		AddResource("unity", "Unity", 80.0f, 12.0f, 150.0f, float3(0.7f, 0.2f, 0.8f));
		AddResource("research", "Research", 0.0f, 0.0f, 0.0f, float3(0.2f, 0.6f, 1.0f));
	}

	void AddResource(const std::string& id, const std::string& name, float current, float perMonth, float capacity, const float3& color) {
		TopBarResource res;
		res.id = id; res.name = name; res.current = current;
		res.perMonth = perMonth; res.capacity = capacity; res.color = color;
		resources.push_back(res);
	}

	void Update(float dt) {
		for (auto& res : resources) {
			res.current += res.perMonth * dt * 0.01f; // monthly tick scaled to per-frame
			if (res.capacity > 0) res.current = std::min(res.current, res.capacity);
			res.current = std::max(0.0f, res.current);
		}
	}

	TopBarResource* GetResource(const std::string& id) {
		for (auto& r : resources) { if (r.id == id) return &r; }
		return nullptr;
	}

	float GetTotalResearch() const {
		float total = 0;
		for (auto& r : resources) { if (r.id.find("research") != std::string::npos) total += r.perMonth; }
		return total;
	}
};

// ======================== Outliner (Right Side Panel) ========================

enum class OutlinerCategory {
	Empire,
	Planets,
	Fleets,
	Armies,
	Construction,
	Trade,
	Military,
};

struct OutlinerEntry {
	std::string id;
	std::string name;
	std::string icon;
	std::string tooltip;
	OutlinerCategory category = OutlinerCategory::Empire;
	int alertLevel = 0; // 0=normal, 1=yellow, 2=red
	bool isCollapsed = false;
	std::function<void()> onClick;
};

class StellarisOutliner {
public:
	std::vector<OutlinerEntry> entries;
	bool isVisible = true;
	bool isExpanded = true;
	float panelWidth = 280.0f;

	void AddEntry(const OutlinerEntry& entry) {
		entries.push_back(entry);
	}

	void RemoveEntry(const std::string& id) {
		entries.erase(
			std::remove_if(entries.begin(), entries.end(),
				[&](const OutlinerEntry& e) { return e.id == id; }),
			entries.end()
		);
	}

	void UpdateEntry(const std::string& id, const std::string& name, int alertLevel) {
		for (auto& e : entries) {
			if (e.id == id) { e.name = name; e.alertLevel = alertLevel; return; }
		}
	}

	void ToggleCategory(OutlinerCategory cat) {
		for (auto& e : entries) {
			if (e.category == cat) e.isCollapsed = !e.isCollapsed;
		}
	}

	std::vector<OutlinerEntry*> GetEntriesByCategory(OutlinerCategory cat) {
		std::vector<OutlinerEntry*> result;
		for (auto& e : entries) {
			if (e.category == cat) result.push_back(&e);
		}
		return result;
	}

	void OnClick(float mx, float my) {
		float y = 0;
		for (auto& e : entries) {
			if (e.isCollapsed) continue;
			if (mx >= 0 && mx <= panelWidth && my >= y && my <= y + 30.0f) {
				if (e.onClick) e.onClick();
				return;
			}
			y += 30.0f;
		}
	}
};

// ======================== Side Panel (Context-sensitive) ========================

enum class PanelType {
	None,
	PlanetView,
	FleetView,
	Diplomacy,
	Research,
	Economy,
	Technology,
	Espionage,
	GalaxyMap,
	Empire,
};

class StellarisSidePanel {
public:
	PanelType currentPanel = PanelType::None;
	bool isVisible = false;
	float panelWidth = 450.0f;
	float animationProgress = 0.0f; // 0=closed, 1=open
	std::string title;
	std::string subtitle;

	void OpenPanel(PanelType type, const std::string& t, const std::string& sub = "") {
		currentPanel = type;
		title = t;
		subtitle = sub;
		isVisible = true;
		animationProgress = 0.0f;
	}

	void ClosePanel() {
		currentPanel = PanelType::None;
		isVisible = false;
		animationProgress = 0.0f;
	}

	void Update(float dt) {
		if (isVisible && animationProgress < 1.0f) {
			animationProgress = std::min(1.0f, animationProgress + dt * 4.0f); // 0.25s open
		}
		if (!isVisible && animationProgress > 0.0f) {
			animationProgress = std::max(0.0f, animationProgress - dt * 6.0f);
		}
	}

	bool IsAnimating() const { return animationProgress > 0.0f && animationProgress < 1.0f; }
};

// ======================== Galaxy Map Overlay ========================

struct MapMarker {
	std::string id;
	std::string name;
	float3 position = ZeroVector;
	std::string type; // "star", "planet", "fleet", "anomaly", "wormhole"
	int ownerTeam = -1;
	float size = 1.0f;
	bool isSelected = false;
};

class StellarisGalaxyMap {
public:
	std::vector<MapMarker> markers;
	float zoom = 1.0f;
	float3 cameraPosition = ZeroVector;
	float3 cameraTarget = ZeroVector;
	bool showEmpireColors = true;
	bool showTradeRoutes = true;
	bool showFleetPaths = true;
	bool showLabels = true;
	float minZoom = 0.1f;
	float maxZoom = 10.0f;

	void AddMarker(const MapMarker& marker) { markers.push_back(marker); }

	void Zoom(float delta) {
		zoom = std::max(minZoom, std::min(maxZoom, zoom + delta));
	}

	void Pan(const float3& delta) {
		cameraPosition += delta * (1.0f / zoom);
	}

	void SelectMarker(const std::string& id) {
		for (auto& m : markers) m.isSelected = (m.id == id);
	}

	MapMarker* GetSelectedMarker() {
		for (auto& m : markers) { if (m.isSelected) return &m; }
		return nullptr;
	}

	std::vector<MapMarker*> GetMarkersInRadius(const float3& center, float radius) {
		std::vector<MapMarker*> result;
		for (auto& m : markers) {
			if ((m.position - center).Length() <= radius) result.push_back(&m);
		}
		return result;
	}

	void ClearSelection() {
		for (auto& m : markers) m.isSelected = false;
	}
};

// ======================== Contextual Tooltip ========================

struct TooltipData {
	std::string title;
	std::string description;
	std::string detailedText;
	std::vector<std::string> modifiers;
	std::vector<std::pair<std::string, float>> stats;
	float3 headerColor = float3(1, 1, 1);
	bool hasWarning = false;
	std::string warningText;
};

class StellarisTooltip {
public:
	TooltipData data;
	bool isVisible = false;
	float showDelay = 0.5f; // seconds before showing
	float delayTimer = 0.0f;
	float2 position = float2(0, 0);

	void Show(const TooltipData& td, float x, float y) {
		data = td;
		position = float2(x, y);
		delayTimer = 0.0f;
		isVisible = false;
	}

	void Hide() { isVisible = false; delayTimer = 0.0f; }

	void Update(float dt) {
		if (!isVisible && delayTimer < showDelay) {
			delayTimer += dt;
			if (delayTimer >= showDelay) isVisible = true;
		}
	}
};

// ======================== Notification System ========================

enum class NotificationType {
	Info,
	Warning,
	Critical,
	Diplomacy,
	Military,
	Economy,
	Technology,
	Achievement,
};

struct Notification {
	std::string id;
	std::string title;
	std::string message;
	NotificationType type = NotificationType::Info;
	float timeRemaining = 5.0f;
	bool isRead = false;
	std::function<void()> onClick;
};

class StellarisNotificationSystem {
public:
	std::vector<Notification> activeNotifications;
	std::vector<Notification> history;
	int maxActive = 5;

	void AddNotification(const Notification& notif) {
		activeNotifications.push_back(notif);
		if (static_cast<int>(activeNotifications.size()) > maxActive) {
			history.push_back(activeNotifications.front());
			activeNotifications.erase(activeNotifications.begin());
		}
	}

	void Update(float dt) {
		for (auto& n : activeNotifications) {
			n.timeRemaining -= dt;
		}
		activeNotifications.erase(
			std::remove_if(activeNotifications.begin(), activeNotifications.end(),
				[](const Notification& n) { return n.timeRemaining <= 0; }),
			activeNotifications.end()
		);
	}

	void Dismiss(const std::string& id) {
		for (auto it = activeNotifications.begin(); it != activeNotifications.end(); ++it) {
			if (it->id == id) { activeNotifications.erase(it); return; }
		}
	}
};

// ======================== Main Stellaris UI Manager ========================

class StellarisUI {
public:
	StellarisTopBar topBar;
	StellarisOutliner outliner;
	StellarisSidePanel sidePanel;
	StellarisGalaxyMap galaxyMap;
	StellarisTooltip tooltip;
	StellarisNotificationSystem notifications;

	bool showTutorial = true;
	bool showAdvancedTooltips = false;
	float hudOpacity = 1.0f;

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

	void ToggleOutliner() { outliner.isExpanded = !outliner.isExpanded; }

	void OpenPlanetView(const std::string& planetName) {
		sidePanel.OpenPanel(PanelType::PlanetView, planetName, "Planet Overview");
	}

	void OpenFleetView(const std::string& fleetName) {
		sidePanel.OpenPanel(PanelType::FleetView, fleetName, "Fleet Management");
	}

	void OpenDiplomacy(const std::string& empireName) {
		sidePanel.OpenPanel(PanelType::Diplomacy, empireName, "Diplomatic Relations");
	}

	void OpenResearch() {
		sidePanel.OpenPanel(PanelType::Research, "Research", "Technologies & Discoveries");
	}

	void OpenEconomy() {
		sidePanel.OpenPanel(PanelType::Economy, "Economy", "Trade & Resources");
	}
};

} // namespace arclight
