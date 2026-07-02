# How To Code For The Engine

Use this guide when writing gameplay scripts or integrating Lua with engine code.

## Rules

- Keep logic small and focused.
- Prefer modules over global state.
- Keep engine code in C++ and gameplay behavior in Lua.
- Use OOP-style Lua classes for reusable gameplay objects.
- Document each new system with a short README or UML note.

## Suggested flow

1. Create the script module.
2. Add the runtime registration.
3. Add a gameplay example.
4. Write a short doc or UML diagram.
