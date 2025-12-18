# CryptRover TypeScript: Code Tour

## Build and Run
```bash
npm run dev    # Watch + serve at http://localhost:8080
npm run build  # One-time build to dist/
npm run test   # Run test suite
```

## Data Shapes (globals)
- **Map grids**: `tileM[Y_][X_]`, `tileColorM`, `viewM` in [src-ts/map.ts](src-ts/map.ts)
- **Entities**: `entL[ENTS_]` (player at index 0) and occupancy `entM[Y_][X_]` in [src-ts/entities.ts](src-ts/entities.ts)
- **Items**: `itemL[ITEMS_]` and placement `itemM[Y_][X_]` in [src-ts/items.ts](src-ts/items.ts)

## Core Loop ([src-ts/main.ts](src-ts/main.ts))
- **init()**: Seeds RNG, creates canvas, registers input, inits audio, generates map/entities/items, computes FOV, draws, shows help, enters gameLoop.
- **gameLoop()** async turn sequence:
  1. `waitForInput()` blocks for player action
  2. `processAction()` handles move/wait/toggle/stair/help/quit
  3. `useItem()` auto-applies item at player tile
  4. `moveAllEnemies()` advances all arachnids (speed-gated)
  5. Check player death (HP or air ≤ 0)
  6. `decayFov()` converts IN_SIGHT → SEEN
  7. Drain battery if light on; drain air (warn at 10)
  8. `fov()` recomputes visibility
  9. `drawScreen()` + `updateHUD()`
- **End conditions**: Descend past LAST_LEVEL → win; HP/air ≤ 0 or quit → loss; both trigger `saveScore()` + `showGameOver()`.

## Map Generation ([src-ts/map.ts](src-ts/map.ts))
- **initMap()**: Fills walls, clears view, calls `digLevel()`, places single `<` stair on random floor.
- **digLevel()**: Recursive room-corridor carving from center outward. Each room spawns 1–PATHS child rooms at random offsets; connects via Bresenham corridors.
- **hasSpace()**: Validates 1-tile buffer around proposed room.
- **digRoom()**: Carves circular (radial) or rectangular rooms.
- **los(y0,x0,y1,x1,apply?)**: Bresenham LOS with optional per-tile callback.

| Export | Purpose |
|--------|---------|
| `initMap()` | Generate new level |
| `getRandomFloor()` | Random floor coords for spawning |
| `isWalkable(y,x)` | Check FLOOR/NEXT_LEVEL |
| `los()` | Line-of-sight check |
| `tileM`, `tileColorM`, `viewM` | Map state |

## Entities and AI ([src-ts/entities.ts](src-ts/entities.ts))
- **Ent**: id, x/y, hp, air, battery, coins, speed, awake, lightOn, alive, glyph, color.
- **initEnts(level)**: Clears `entM`, places player (id=0), spawns 11 arachnids avoiding walls/overlap/near-player. Player stats set only on level 1.
- **fov(y,x,radius)**: Marks tiles IN_SIGHT within battery-scaled radius if LOS clear.
- **moveTo(ent,dy,dx)**: Movement + combat. On collision with living entity: deal 1 damage, handle death (blood splatter 3×3), low-HP warning.
- **moveEnemy()**: Wakes on LOS within FOV_RADIUS; if awake, pathfinds toward player (sort adjacent by distance); else random move.
- **moveAllEnemies(turn)**: Iterates enemies 1–11 respecting speed gates (`turn % speed === 0`).

| Export | Purpose |
|--------|---------|
| `initEnts(level)` | Spawn player + enemies |
| `getPlayer()` | Returns `entL[0]` |
| `fov(y,x,r)` | Compute visibility |
| `moveTo(ent,dy,dx)` | Move/attack |
| `moveEnemy(e,p)` | Single enemy AI |
| `moveAllEnemies(turn)` | Process all enemies |
| `isPlayerAlive()` | HP > 0 && air > 0 |
| `entL`, `entM` | Entity state |

## Items and Pickup ([src-ts/items.ts](src-ts/items.ts))
- **Item**: x/y, glyph, type, color, used.
- **initItems()**: Scatters items on floor avoiding stairs/entities/items. Order: med packs, air cans, batteries, coins.
- **useItem(player)**: Auto-triggers on player tile. Heals/restores up to caps; coins always picked. Marks `used=true`, removes from `itemM`.

| Type | Glyph | Count | Charge |
|------|-------|-------|--------|
| med | `+` | 3 | +3 HP |
| air | `~` | 4 | +21 air |
| battery | `=` | 5 | +36 battery |
| coin | `$` | 3 | +1 coin |

## Rendering and UI ([src-ts/io.ts](src-ts/io.ts))
- **Canvas**: 16×16 cells, pixel-perfect rendering. `drawScreen()` in main.ts iterates grid, draws tiles/items/corpses/entities based on view state.
- **Input**: `initInput()` registers keydown; `waitForInput()` returns Promise<InputAction>. Vi keys, WASD, arrows, numpad supported.
- **HUD**: `updateHUD()` renders HP/air/battery bars, coins, level, light status to DOM.
- **Messages**: `addMessage(text,class)` with 50-message buffer, auto-scroll.
- **Modals**: `showHelp()`/`hideHelp()`, `showGameOver(won,stats)` with high score table from localStorage.

| Export | Purpose |
|--------|---------|
| `waitForInput()` | Async input Promise |
| `initInput()` | Register listeners |
| `showHelp/hideHelp()` | Help modal |
| `showGameOver()` | End screen + scores |
| `addMessage()` | Log message |
| `updateHUD()` | Refresh sidebar |

## Types ([src-ts/types.ts](src-ts/types.ts))
- **ViewState**: `UNSEEN | SEEN | IN_SIGHT`
- **Ent**, **Item**, **Tile**, **Point**: Core interfaces
- **DIRECTIONS**: 8-directional movement vectors
- **makeGrid(y,x,init)**: 2D array factory

## Utilities ([src-ts/utils.ts](src-ts/utils.ts))
- **PRNG**: Mulberry32 via `initRng()`, `setSeed()`, `randInt(lo,hi)`
- **Math**: `min`, `max`, `abs`, `clamp`, `swap`
- **Geometry**: `dist2()` squared distance, `inRange()` radius check, `bresenham()` line points

## Constants ([src-ts/constants.ts](src-ts/constants.ts))
| Category | Values |
|----------|--------|
| Map | Y_=24, X_=48, CELL_SIZE=16, ROOM_RADIUS=2, PATHS=5, LAST_LEVEL=12 |
| Tiles | WALL='#', FLOOR='.', NEXT_LEVEL='<' |
| View | UNSEEN=0, SEEN=1, IN_SIGHT=2 |
| Entities | ENTS_=12, FOV_RADIUS=5, PLAYER='@', ARACHNID='a', CORPSE='%' |
| Player | HP=18, AIR=105, BATTERY=180 |
| Items | MED_NUM=3, AIR_NUM=4, BATTERY_NUM=5, COIN_NUM=3 |
| Charges | MED=3, AIR=21, BATTERY=36, COIN=1 |
| Colors | CSS hex map (black, white, red, green, blue, cyan, yellow, magenta, gray) |

## Audio ([src-ts/audio.ts](src-ts/audio.ts))
Stub API for Web Audio (not yet implemented):
- `playSound(name)`: 'money', 'clunk', 'bite', 'punch', 'grunt', 'alert'
- `playMusic(name)`: 'theme'
- `initAudio()`, `stopMusic()`, `cleanupAudio()`

## File Summary

| File | Role |
|------|------|
| [main.ts](src-ts/main.ts) | Game loop, init, win/loss, canvas draw |
| [map.ts](src-ts/map.ts) | Room/corridor generation, LOS |
| [entities.ts](src-ts/entities.ts) | FOV, AI, combat, movement |
| [items.ts](src-ts/items.ts) | Spawn and pickup |
| [io.ts](src-ts/io.ts) | Input, HUD, modals, messages |
| [types.ts](src-ts/types.ts) | Core interfaces and grid factory |
| [utils.ts](src-ts/utils.ts) | PRNG, math, Bresenham |
| [constants.ts](src-ts/constants.ts) | Game parameters and colors |
| [audio.ts](src-ts/audio.ts) | Audio stubs |
