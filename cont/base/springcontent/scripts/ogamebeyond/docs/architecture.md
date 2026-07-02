# Architecture Notes

This Lua scaffold uses a simple split:

- `engine/` for loop, event bus, and timing
- `game/` for universe state and simulation rules
- `ui/` for layout and presentation layers

The layout is original and inspired by large-scale space strategy interfaces.
