# UML Sketch

```mermaid
classDiagram
    class Engine
    class Universe
    class GameState
    class UI
    class Planet
    class Fleet
    class Economy
    class Research

    Engine --> Universe
    Engine --> GameState
    Engine --> UI
    Universe --> Planet
    Universe --> Fleet
    Universe --> Economy
    Universe --> Research
    UI --> GameState
```
