/* ArcLight Engine - Space Strategy HUD Theme
 * Developer: Stephen
 * Neon sci-fi HUD palette inspired by high-contrast space strategy interfaces.
 */

#pragma once

#include <string>

namespace arclight {

struct SpaceHUDColor {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};

struct SpaceHUDTheme {
	SpaceHUDColor background = {0.02f, 0.05f, 0.09f, 0.94f};
	SpaceHUDColor panel = {0.04f, 0.09f, 0.15f, 0.90f};
	SpaceHUDColor panelEdge = {0.18f, 0.47f, 0.75f, 1.0f};
	SpaceHUDColor accent = {0.20f, 0.73f, 0.96f, 1.0f};
	SpaceHUDColor accentAlt = {0.62f, 0.34f, 0.96f, 1.0f};
	SpaceHUDColor text = {0.86f, 0.93f, 0.99f, 1.0f};
	SpaceHUDColor textMuted = {0.53f, 0.64f, 0.74f, 1.0f};
	SpaceHUDColor warning = {0.96f, 0.72f, 0.16f, 1.0f};
	SpaceHUDColor danger = {0.96f, 0.28f, 0.20f, 1.0f};
	SpaceHUDColor success = {0.24f, 0.82f, 0.43f, 1.0f};
	std::string fontFamily = "Orbitron";
	float panelOpacity = 0.92f;
	float glowStrength = 0.35f;
};

} // namespace arclight
