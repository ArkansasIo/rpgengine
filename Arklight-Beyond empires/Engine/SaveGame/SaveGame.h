/* ArkLight Beyond - Save/Load System
 * Developer: Stephen
 * Game state serialization with compression and versioning.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <functional>

namespace ogb {
struct SaveInfo {
    std::string slotName;
    std::string description;
    std::string mapName;
    std::string gameDate;
    int playTimeSeconds = 0;
    int slotIndex = 0;
    bool isAutosave = false;
    uint32_t version = 1;
};

class SaveSystem {
public:
    static constexpr int MAX_SLOTS = 30;
    static constexpr int AUTOSAVE_INTERVAL = 300;

    void Init() {
        for (int i = 0; i < MAX_SLOTS; i++) saves[i].slotIndex = i;
    }

    bool SaveGame(int slot, const std::string& name = "") {
        if (slot < 0 || slot >= MAX_SLOTS) return false;
        SaveInfo& info = saves[slot];
        info.slotName = name.empty() ? "Save " + std::to_string(slot) : name;
        info.gameDate = GetCurrentTimestamp();
        if (onSave) onSave(info);
        return true;
    }

    bool LoadGame(int slot) {
        if (slot < 0 || slot >= MAX_SLOTS) return false;
        if (saves[slot].slotName.empty()) return false;
        if (onLoad) onLoad(saves[slot]);
        return true;
    }

    void DeleteSave(int slot) {
        if (slot >= 0 && slot < MAX_SLOTS) saves[slot] = SaveInfo();
    }

    const SaveInfo& GetSaveInfo(int slot) const { return saves[slot]; }

    void Update(float dt) {
        timeSinceAutosave += dt;
        if (timeSinceAutosave >= AUTOSAVE_INTERVAL) {
            timeSinceAutosave = 0;
            Autosave();
        }
    }

    void Autosave() {
        for (int i = 0; i < MAX_SLOTS; i++) {
            if (saves[i].isAutosave || saves[i].slotName.empty()) {
                saves[i].slotName = "Autosave";
                saves[i].isAutosave = true;
                saves[i].gameDate = GetCurrentTimestamp();
                if (onSave) onSave(saves[i]);
                return;
            }
        }
    }

    std::function<void(const SaveInfo&)> onSave;
    std::function<void(const SaveInfo&)> onLoad;

private:
    SaveInfo saves[MAX_SLOTS];
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
} // namespace ogb
