/* ArcLight Engine - Save/Load System
 * Developer: Stephen
 * Handles game state serialization, autosaves, and manual saves.
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <ctime>

namespace arclight {

struct SaveMetadata {
	std::string saveName;
	std::string description;
	std::string mapName;
	std::string gameTime;
	std::string timestamp;
	int playTimeSeconds = 0;
	int slotIndex = 0;
	bool isAutosave = false;
};

class SaveLoadSystem {
public:
	static constexpr int MAX_SAVE_SLOTS = 20;
	static constexpr int AUTOSAVE_INTERVAL = 300; // 5 minutes

	void Init() {
		for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
			saves[i].slotIndex = i;
		}
	}

	bool SaveGame(int slot, const std::string& name = "") {
		if (slot < 0 || slot >= MAX_SAVE_SLOTS) return false;

		SaveMetadata& meta = saves[slot];
		meta.saveName = name.empty() ? "Save " + std::to_string(slot) : name;
		meta.slotIndex = slot;
		meta.timestamp = GetCurrentTimestamp();
		meta.isAutosave = false;

		if (onSave) onSave(meta);
		return true;
	}

	bool LoadGame(int slot) {
		if (slot < 0 || slot >= MAX_SAVE_SLOTS) return false;
		if (saves[slot].saveName.empty()) return false;

		if (onLoad) onLoad(saves[slot]);
		return true;
	}

	void Autosave() {
		// Find oldest autosave slot or use dedicated autosave slot
		for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
			if (saves[i].isAutosave || saves[i].saveName.empty()) {
				saves[i].saveName = "Autosave";
				saves[i].isAutosave = true;
				saves[i].timestamp = GetCurrentTimestamp();
				if (onSave) onSave(saves[i]);
				return;
			}
		}
	}

	void DeleteSave(int slot) {
		if (slot >= 0 && slot < MAX_SAVE_SLOTS) {
			saves[slot] = SaveMetadata();
			saves[slot].slotIndex = slot;
		}
	}

	const SaveMetadata& GetSaveInfo(int slot) const {
		return saves[slot];
	}

	void Update(float dt) {
		timeSinceAutosave += dt;
		if (timeSinceAutosave >= AUTOSAVE_INTERVAL) {
			timeSinceAutosave = 0;
			Autosave();
		}
	}

	std::function<void(const SaveMetadata&)> onSave;
	std::function<void(const SaveMetadata&)> onLoad;

private:
	SaveMetadata saves[MAX_SAVE_SLOTS];
	float timeSinceAutosave = 0;

	std::string GetCurrentTimestamp() {
		time_t now = time(nullptr);
		struct tm tm;
#ifdef _WIN32
		localtime_s(&tm, &now);
#else
		tm = *localtime(&now);
#endif
		char buf[64];
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
		return buf;
	}
};

} // namespace arclight
