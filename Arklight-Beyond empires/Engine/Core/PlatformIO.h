/* ArkLight Beyond - Platform I/O
 * Developer: Stephen
 * File system abstraction, async I/O, and platform-specific storage.
 */
#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace ogb {

enum class EFileMode { Read, Write, Append, ReadBinary, WriteBinary };
enum class ESeekMode { Begin, Current, End };

struct FileHandle {
    void* handle = nullptr;
    std::string path;
    EFileMode mode = EFileMode::Read;
    bool isOpen = false;
    size_t size = 0;
};

class PlatformIO {
public:
    // File operations
    static FileHandle OpenFile(const std::string& path, EFileMode mode) {
        FileHandle fh;
        fh.path = path;
        fh.mode = mode;
        fh.isOpen = true;
        return fh;
    }

    static void CloseFile(FileHandle& fh) { fh.isOpen = false; fh.handle = nullptr; }

    static size_t ReadFile(FileHandle& fh, void* buffer, size_t bytes) {
        return 0; // platform implementation
    }

    static size_t WriteFile(FileHandle& fh, const void* buffer, size_t bytes) {
        return bytes; // platform implementation
    }

    static void SeekFile(FileHandle& fh, size_t offset, ESeekMode mode) {}

    static size_t GetFileSize(const std::string& path) { return 0; }
    static bool FileExists(const std::string& path) { return false; }
    static bool DirectoryExists(const std::string& path) { return false; }

    static void CreateDirectory(const std::string& path) {}
    static void DeleteFile(const std::string& path) {}
    static void RenameFile(const std::string& from, const std::string& to) {}

    static std::vector<std::string> ListFiles(const std::string& path, const std::string& filter = "*") {
        return {};
    }

    static std::vector<std::string> ListDirectories(const std::string& path) { return {}; }

    // Async I/O
    struct AsyncReadRequest {
        uint64_t id = 0;
        std::string path;
        std::function<void(const std::vector<uint8_t>&)> onComplete;
        bool isComplete = false;
    };

    static uint64_t ReadFileAsync(const std::string& path, std::function<void(const std::vector<uint8_t>&)> callback) {
        return 0;
    }

    // Platform-specific paths
    static std::string GetSaveDirectory() {
#ifdef _WIN32
        return "C:/Users/" + GetUserName() + "/Documents/My Games/OGameBeyond/";
#elif __APPLE__
        return "~/Library/Application Support/OGameBeyond/";
#elif __linux__
        return "~/.local/share/OGameBeyond/";
#elif __ps5__
        return "/data/appmeta/OGameBeyond/savedata/";
#elif __XBOX_SERIES_X__ || __XBOX_SERIES_S__
        return "E:/Games/OGameBeyond/savedata/";
#elif __SWITCH__
        return "/save/OGameBeyond/";
#else
        return "./saves/";
#endif
    }

    static std::string GetCacheDirectory() { return GetSaveDirectory() + "cache/"; }
    static std::string GetLogDirectory() { return GetSaveDirectory() + "logs/"; }
    static std::string GetConfigDirectory() { return GetSaveDirectory() + "config/"; }

    // Path utilities
    static std::string CombinePath(const std::string& a, const std::string& b) {
        if (a.empty()) return b;
        if (a.back() == '/' || a.back() == '\\') return a + b;
        return a + OGB_PATH_SEP + b;
    }

    static std::string GetExtension(const std::string& path) {
        size_t dot = path.rfind('.');
        return (dot != std::string::npos) ? path.substr(dot) : "";
    }

    static std::string GetFileName(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        return (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
    }

    static std::string GetDirectoryName(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        return (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";
    }

private:
    static std::string GetUserName() { return "User"; }
};

} // namespace ogb
