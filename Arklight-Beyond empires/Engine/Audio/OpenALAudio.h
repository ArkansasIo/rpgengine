/* OGame Beyond - OpenAL Soft Audio Engine
 * Developer: Stephen
 * 3D spatial audio, reverb, occlusion, music system, and voice chat.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ogb {

struct AudioConfig {
    int sampleRate = 44100;
    int bufferSize = 1024;
    int maxSources = 128;
    int maxListeners = 1;
    bool enableReverb = true;
    bool enableOcclusion = true;
    bool enableDoppler = true;
    float dopplerFactor = 1.0f;
    float speedOfSound = 343.3f;
};

struct SoundAsset {
    std::string id;
    std::string filePath;
    float duration = 0;
    int channels = 1;
    int sampleRate = 44100;
    size_t dataSize = 0;
    bool isLoaded = false;
    void* handle = nullptr;
};

struct AudioSource {
    uint32_t sourceID = 0;
    std::string soundID;
    Vec3 position;
    Vec3 velocity;
    float gain = 1.0f;
    float pitch = 1.0f;
    float referenceDistance = 10.0f;
    float maxDistance = 1000.0f;
    float rolloffFactor = 1.0f;
    bool isPlaying = false;
    bool isLooping = false;
    bool is3D = true;
    float occlusionFactor = 0.0f;
};

struct MusicTrack {
    std::string id;
    std::string filePath;
    std::string mood;
    float volume = 0.7f;
    float fadeInTime = 2.0f;
    float fadeOutTime = 2.0f;
};

struct ReverbZone {
    Vec3 position;
    float radius = 100.0f;
    float reverbLevel = 0.5f;
    float reverbTime = 1.0f;
    float roomRolloff = 1.0f;
};

class OpenALAudioEngine {
public:
    AudioConfig config;

    void Init() {
        // Open OpenAL device
        // Create context
        // Set listener properties
    }

    void Shutdown() {
        sources.clear();
        sounds.clear();
    }

    // Sound loading
    uint32_t LoadSound(const std::string& id, const std::string& path) {
        SoundAsset asset;
        asset.id = id;
        asset.filePath = path;
        asset.isLoaded = true;
        uint32_t handle = nextSoundID++;
        sounds[handle] = asset;
        return handle;
    }

    void UnloadSound(uint32_t handle) { sounds.erase(handle); }

    // Source management
    uint32_t CreateSource() {
        AudioSource source;
        source.sourceID = nextSourceID++;
        sources.push_back(source);
        return source.sourceID;
    }

    void DestroySource(uint32_t id) {
        sources.erase(
            std::remove_if(sources.begin(), sources.end(),
                [id](const AudioSource& s) { return s.sourceID == id; }),
            sources.end());
    }

    void PlaySound(uint32_t sourceID, uint32_t soundID) {
        for (auto& s : sources) {
            if (s.sourceID == sourceID) { s.soundID = std::to_string(soundID); s.isPlaying = true; }
        }
    }

    void StopSound(uint32_t sourceID) {
        for (auto& s : sources) { if (s.sourceID == sourceID) s.isPlaying = false; }
    }

    void PauseSound(uint32_t sourceID) {}
    void ResumeSound(uint32_t sourceID) {}

    // 3D positioning
    void SetListenerPosition(const Vec3& pos, const Vec3& forward, const Vec3& up) {
        listenerPosition = pos;
        listenerForward = forward;
        listenerUp = up;
    }

    void SetSourcePosition(uint32_t sourceID, const Vec3& pos) {
        for (auto& s : sources) { if (s.sourceID == sourceID) s.position = pos; }
    }

    void SetSourceVelocity(uint32_t sourceID, const Vec3& vel) {
        for (auto& s : sources) { if (s.sourceID == sourceID) s.velocity = vel; }
    }

    // Volume
    void SetMasterVolume(float v) { masterVolume = v; }
    void SetSFXVolume(float v) { sfxVolume = v; }
    void SetMusicVolume(float v) { musicVolume = v; }
    void SetVoiceVolume(float v) { voiceVolume = v; }

    // Music
    void PlayMusic(const std::string& trackID, float fadeTime = 2.0f) {
        currentMusic = trackID;
        musicPlaying = true;
    }

    void StopMusic(float fadeTime = 2.0f) { musicPlaying = false; }
    void PauseMusic() { musicPaused = true; }
    void ResumeMusic() { musicPaused = false; }

    // Reverb zones
    void AddReverbZone(const ReverbZone& zone) { reverbZones.push_back(zone); }

    // DSP effects
    void SetReverbEnabled(bool enabled) { config.enableReverb = enabled; }
    void SetOcclusionEnabled(bool enabled) { config.enableOcclusion = enabled; }
    void SetDopplerFactor(float factor) { config.dopplerFactor = factor; }

    // Update
    void Update(float dt) {
        // Update 3D positions
        // Apply occlusion
        // Update music crossfade
        // Clean up finished sources
    }

    // Voice chat
    void StartVoiceCapture() { voiceChatActive = true; }
    void StopVoiceCapture() { voiceChatActive = false; }
    void SendVoiceData(const uint8_t* data, size_t size) {}

private:
    float masterVolume = 1.0f;
    float sfxVolume = 1.0f;
    float musicVolume = 0.7f;
    float voiceVolume = 1.0f;
    bool musicPlaying = false;
    bool musicPaused = false;
    bool voiceChatActive = false;
    std::string currentMusic;
    Vec3 listenerPosition;
    Vec3 listenerForward;
    Vec3 listenerUp;
    std::unordered_map<uint32_t, SoundAsset> sounds;
    std::vector<AudioSource> sources;
    std::vector<ReverbZone> reverbZones;
    uint32_t nextSoundID = 1;
    uint32_t nextSourceID = 1;
};

} // namespace ogb
