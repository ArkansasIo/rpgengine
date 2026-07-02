# Space HUD Scaffold

This module sketches a space strategy UI inspired by the provided reference image.

## Layout

- top resource strip
- left empire/navigation rail
- center tactical viewport
- right operations stack
- bottom build and ship deck
- compact minimap and chat zones

## Files

- `SpaceHUDTheme.h` - visual theme and palette values
- `SpaceHUDLayout.h` - panel bounds, resources, queue data, and layout presets
- `SpaceHUD.h` - orchestration stub for runtime integration

This is a scaffold, not a finished renderer. It is designed so the HUD can be wired into `CGuiHandler` or a Lua-driven front end in small increments.

## Resource Focus Modes

- clicking a resource chip changes the active economy focus
- the bottom deck shows a filtered build queue for the current focus
- the side rail swaps to a focus-specific navigation subview
- the galaxy panel filters markers to the selected economy theme
