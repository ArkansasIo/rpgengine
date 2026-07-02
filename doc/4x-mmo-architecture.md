# ArcLight 4X RTS/MMO Architecture

This module turns the attached OGame + Stellaris feature list into engine-facing slices that can be built and tested incrementally.

## First Supported Slice

- `rts/GrandStrategy/Types.h`: deterministic IDs, fixed-point resources, star/planet/fleet enums.
- `rts/GrandStrategy/Galaxy.h/.cpp`: deterministic procedural galaxy generation from a seed.
- `rts/GrandStrategy/Empire.h`: empire identity, government, diplomacy stance, treasury, colonies, fleets.
- `rts/GrandStrategy/Economy.h`: monthly stockpile/income/upkeep accounting.
- `rts/GrandStrategy/Fleet.h`: ship designs, fleet ownership, location, and orders.
- `rts/GrandStrategy/Simulation.h`: server-style tick shell for persistent simulation.

## Module Map

- Core: IDs, deterministic RNG, fixed-point values, serialization hooks.
- Galaxy: galaxies, systems, planets, anomalies, megastructures.
- Empire: government, ethics/civics later, influence, reputation, stability.
- Economy: resources, logistics, trade routes, markets.
- Fleet: ship classes, ship designer, fleet orders, logistics.
- Combat: tactical RTS battles, orbital bombardment, ground combat.
- Diplomacy: treaties, alliances, federation/senate systems.
- Population: species, jobs, housing, migration, governors.
- Research: branching tech tree, reverse engineering, artifacts.
- Multiplayer: persistent server state, cluster handoff, replay verification.
- Modding: Lua/C++ plugins, live reload, dependency metadata.
- Editor: galaxy, planet, ship, material, particle, tech, campaign, quest tools.

## Determinism Rules

- Grand-strategy simulation code should avoid floating-point state changes where sync matters.
- Economy values use `FixedPoint`.
- Procedural generation uses `DeterministicRng`, seeded from server/save data.
- Network clients should receive commands and authoritative snapshots, not run divergent random generation.

## Next Implementation Steps

- Add binary/text serialization for `SimulationState`.
- Add Lua bindings for galaxy generation and empire commands.
- Add EnTT-backed runtime entities for fleets, colonies, and construction queues.
- Add dedicated server persistence snapshots and replay checkpoints.
- Add renderer-facing adapters for galaxy/planet map visualization.
