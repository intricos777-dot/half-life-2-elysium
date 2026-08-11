# Half-Life 2 Elysium

Fan-engine recreation project targeting the feel, structure, and atmosphere of the original title, built on the Twilight Elysium engine.

> This is an independent fan project. It is not affiliated with, endorsed by, or connected to Valve or Half-Life 2 in any way.

## Goals
- Rebuild the City 17 journey, physics-driven arsenal, and Combine threat in Elysium
- Tool-assisted asset generation and audio-reactive rendering
- Local-only engine runtime with optional editor tooling

## Layout
- `Content/` — maps, data scripts, generated assets
- `Source/` — engine + game modules
- `tools/` — asset pipeline, dream engine, launcher
- `tests/` — smoke and regression tests
- `docs/` — design notes

## Difficulties
Identical casual/legend profiles to every other Elysium build (`Content/Data/Difficulties.json`):
- `casual` — forgiving damage, extra ammo, fast respawns
- `legend` — lethal enemies, scarce supplies, no mercy

```bash
python3 tools/hl2_dream.py casual   # relaxed dream run
python3 tools/hl2_dream.py legend   # brutal dream run
```

## Build
TBD based on engine language choice; current prototype uses Python renderer + optional C++ backend.
