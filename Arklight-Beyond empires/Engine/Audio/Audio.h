/* OGame Beyond - Audio Engine
 * Developer: Stephen
 * 3D spatial audio, music system, and sound effects.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {
struct SoundDef {
    std::string id;
    std::string path;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool is3D = false;
    bool looping = false;
};

struct SoundInstance {
    int id = -1;
    std::string soundID;
    Vec3 position;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool isPlaying = false;
    bool isLooping = false;
    float fadeTime = 0;
};

struct MusicTrack {
    std::string id;
    std::string path;
    float volume = 0.7f;
    std::string mood; // "peace", "battle", "explore"
};

class AudioSystem {
public:
    void Init() {}
    void Shutdown() {}

    int PlaySound(const std::string& soundID, const Vec3& pos = VEC3_ZERO, float vol = 1.0f) {
        int id = nextSoundID++;
        SoundInstance inst;
        inst.id = id;
        inst.soundID = soundID;
        inst.position = pos;
        inst.volume = vol;
        inst.isPlaying = true;
        instances.push_back(inst);
        return id;
    }

    void StopSound(int id) {
        for (auto& inst : instances) {
            if (inst.id == id) { inst.isPlaying = false; return; }
        }
    }

    void SetMasterVolume(float v) { masterVolume = v; }
    void SetMusicVolume(float v) { musicVolume = v; }
    void SetSFXVolume(float v) { sfxVolume = v; }

    void PlayMusic(const std::string& trackID) {
        currentMusic = trackID;
        musicPlaying = true;
    }

    void StopMusic() { musicPlaying = false; }

    void Update(float dt) {
        // Clean up finished sounds
        instances.erase(
            std::remove_if(instances.begin(), instances.end(),
                [](const SoundInstance& s) { return !s.isPlaying; }),
            instances.end());
    }

private:
    float masterVolume = 1.0f;
    float musicVolume = 0.7f;
    float sfxVolume = 1.0f;
    bool musicPlaying = false;
    std::string currentMusic;
    std::vector<SoundInstance> instances;
    int nextSoundID = 1;
};
} // namespace ogb
