# CLAUDE.md

## Overview

Terminal roguelike in C99 + ncurses (optional SDL_mixer audio). TypeScript/browser port planned.

**Detailed documentation**: [code.md](code.md) (architecture), [game.md](game.md) (game design), [ts-plan.md](ts-plan.md) (web port).

## Environment

**Runtime**: Optional `media/*.wav`/`*.ogg` for audio. Scores written to `scores.dat`.

## Architecture

**Globals** (defined in [src/main.c](src/main.c)):
- `tile_m[Y_][X_]`, `tile_color_m`, `view_m` — map grids
- `ent_l[ENTS_]`, `ent_m[Y_][X_]` — entities (player at index 0)
- `item_l[ITEMS_]`, `item_m[Y_][X_]` — items

**Modules**:
| File | Role |
|------|------|
| `main.c` | Game loop, init, win/loss |
| `map.c` | Room/corridor generation |
| `entities.c` | FOV, AI, combat |
| `items.c` | Spawn and pickup |
| `io.c` | Rendering, HUD, modals |
| `mdport.c` | Input normalization |
| `utils.c` | Math, Bresenham LOS |

**Turn sequence**: `player_action()` → `use_item()` → enemy moves → FOV decay → drain battery/air → recompute FOV → redraw.

## Domain Context

- Map: 24×48, 12 levels, FOV radius 5 (battery-scaled)
- Player: HP 18, air 105, battery 180
- Per level: 11 arachnids, 3 med packs, 4 air cans, 3 batteries, 5 coins
- SDL audio guarded by `__SDL__` define; gracefully skipped otherwise

## Coding Standards

- Use `add_message()` for player feedback
- Declare `extern` in headers, define once in `.c`
- Colors via `COLOR_PAIR` macros in `io.h`
- Input changes go in `mdport.c`, not scattered elsewhere

## Prohibitions

- Never reveal entities/items outside fog rules
- Never desync `ent_m` when moving entities
- Avoid changing `scores.dat` format without updating both save/read paths
