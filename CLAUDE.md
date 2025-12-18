# CLAUDE.md

## Overview

Terminal roguelike. TypeScript/browser version in `src-ts/`, legacy C99 + ncurses version in `src/`.

**Detailed documentation**: [code.md](code.md) (architecture), [game.md](game.md) (game design), [ts-plan.md](ts-plan.md) (web port).

## Development

```bash
npm run dev    # Watch + serve at http://localhost:8080
npm run test   # Run test suite
npm run build  # One-time build
```

## Architecture (TypeScript)

Source in `src-ts/`, compiles to `dist/`.

| File | Role |
|------|------|
| `main.ts` | Game loop, init, win/loss |
| `map.ts` | Room/corridor generation |
| `entities.ts` | FOV, AI, combat |
| `items.ts` | Spawn and pickup |
| `io.ts` | Canvas rendering, HUD, modals |
| `input.ts` | Keyboard handling |
| `utils.ts` | Math, Bresenham LOS |
| `state.ts` | Global game state |

**Turn sequence**: `playerAction()` → `useItem()` → enemy moves → FOV decay → drain battery/air → recompute FOV → redraw.

## Domain Context

- Map: 24×48, 12 levels, FOV radius 5 (battery-scaled)
- Player: HP 18, air 105, battery 180
- Per level: 11 arachnids, 3 med packs, 4 air cans, 3 batteries, 5 coins

## Coding Standards

- Use `addMessage()` for player feedback
- Keep game state in `state.ts` globals
- Colors defined in `io.ts`

## Prohibitions

- Never reveal entities/items outside fog rules
- Never desync `entMap` when moving entities
