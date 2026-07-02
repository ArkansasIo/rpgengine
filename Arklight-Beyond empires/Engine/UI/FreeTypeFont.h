/* OGame Beyond - FreeType Font Rendering
 * Developer: Stephen
 * Font loading, glyph rendering, and text layout.
 */
#pragma once
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ogb {

struct FontGlyph {
    uint32_t codepoint = 0;
    float u0 = 0, v0 = 0, u1 = 0, v1 = 0; // texture coords
    float width = 0, height = 0;
    float bearingX = 0, bearingY = 0;
    float advance = 0;
    int texturePage = 0;
};

struct FontFace {
    std::string name;
    std::string filePath;
    float size = 16.0f;
    int textureWidth = 1024;
    int textureHeight = 1024;
    std::unordered_map<uint32_t, FontGlyph> glyphs;
    uint32_t textureID = 0;
    float lineHeight = 0;
    float ascender = 0;
    float descender = 0;
    bool isLoaded = false;
};

struct TextLayout {
    std::string text;
    float x = 0, y = 0;
    float fontSize = 16.0f;
    Color color = Color::White();
    float maxWidth = 0; // 0 = no wrap
    float lineSpacing = 1.2f;
    int textAlign = 0; // 0=left, 1=center, 2=right
};

class FreeTypeFontSystem {
public:
    void Init() {}
    void Shutdown() { faces.clear(); }

    bool LoadFont(const std::string& name, const std::string& path, float size = 16.0f) {
        FontFace face;
        face.name = name;
        face.filePath = path;
        face.size = size;
        face.isLoaded = true;
        face.lineHeight = size * 1.2f;
        face.ascender = size * 0.8f;
        face.descender = size * 0.2f;
        faces[name] = face;
        return true;
    }

    FontFace* GetFont(const std::string& name) {
        auto it = faces.find(name);
        return it != faces.end() ? &it->second : nullptr;
    }

    void SetFont(const std::string& name) { currentFont = name; }
    std::string GetCurrentFont() const { return currentFont; }

    // Text measurement
    float MeasureTextWidth(const std::string& text, float fontSize = 16.0f) const {
        return text.size() * fontSize * 0.6f; // approximate
    }

    float MeasureTextHeight(const std::string& text, float maxWidth, float fontSize = 16.0f) const {
        if (maxWidth <= 0) return fontSize * 1.2f;
        float lineWidth = MeasureTextWidth(text, fontSize);
        int lines = static_cast<int>(lineWidth / maxWidth) + 1;
        return lines * fontSize * 1.2f;
    }

    // Text wrapping
    std::vector<std::string> WrapText(const std::string& text, float maxWidth, float fontSize = 16.0f) const {
        std::vector<std::string> lines;
        std::string currentLine;
        float charWidth = fontSize * 0.6f;

        for (char c : text) {
            if (c == '\n') {
                lines.push_back(currentLine);
                currentLine.clear();
            } else {
                currentLine += c;
                if (currentLine.size() * charWidth >= maxWidth) {
                    lines.push_back(currentLine);
                    currentLine.clear();
                }
            }
        }
        if (!currentLine.empty()) lines.push_back(currentLine);
        return lines;
    }

    // Glyph access
    const FontGlyph* GetGlyph(uint32_t codepoint) const {
        auto it = faces.find(currentFont);
        if (it == faces.end()) return nullptr;
        auto git = it->second.glyphs.find(codepoint);
        return git != it->second.glyphs.end() ? &git->second : nullptr;
    }

private:
    std::unordered_map<std::string, FontFace> faces;
    std::string currentFont = "default";
};

} // namespace ogb
