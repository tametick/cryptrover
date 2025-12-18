# Web Port Plan (HTML/CSS/TypeScript)

## Implementation TODO
- [x] Set up HTML/CSS skeleton, tsconfig, strict build (no any), and static serve script.
- [x] Add minimal seedable test harness (`tests/harness.ts`, `npm test`).
- [ ] Port utilities (`dist2`, `inRange`, `bresenham`, RNG) with tests if added.
- [ ] Implement map generation and visualize rooms/paths on canvas grid.
- [ ] Wire base rendering loop and key input normalization.
- [ ] Port entity init, player movement, wall collision, and occupancy map.
- [ ] Implement FOV/LOS and fog rendering.
- [ ] Add enemy AI, speed gate, combat, blood tinting.
- [ ] Add item spawn/use with placement map.
- [ ] Build HUD, message log, end-game flow.
- [ ] Add help modal, quit/win handling, and audio stubs.

Goal: replicate CryptRover in browser with minimal dependencies (no frameworks). Keep structure and naming spirit of C code while using idiomatic TypeScript and simple code.

## Overall Architecture
- Single-page app: one HTML file with a `<canvas>` (for map) and simple HUD/sidebar elements; minimal CSS for layout and colors.
- TypeScript compiled to ES module; no bundler required (tsc + outDir). Keep files small and mirrored to C modules.
- Modules: `map.ts`, `entities.ts`, `items.ts`, `io.ts`, `utils.ts`, `main.ts`, plus small `audio.ts` stubs.
- State kept in module-level variables similar to C globals: tile grids, entity lists, item lists, RNG seed.

## Data Shapes
- Constants mirror C (names in UPPER_SNAKE_CASE): `Y_ = 24`, `X_ = 48`, tile glyph enums (`WALL`, `FLOOR`, `NEXT_LEVEL`), counts (`ENTS_`, `ITEMS_`, `LAST_LEVEL`, etc.).
- Types:
  - `Tile` enum for map glyphs; separate `color` and `view` arrays typed as `number[][]` or typed aliases.
  - `Ent` interface: `id`, `x`, `y`, `hp`, `air`, `battery`, `coins`, `speed`, `awake`, `lightOn`, `alive`, `glyph`, `color`.
  - `Item` interface: `x`, `y`, `glyph`, `type`, `color`, `used`.
- Grids: `tileM`, `tileColorM`, `viewM`, `entM`, `itemM` as 2D arrays of numbers; helper `makeGrid<T>(height, width, init)`.

## Utilities (`utils.ts`)
- Math helpers: `min`, `max`, `swap`, `dist2(ax, ay, bx, by)`, `inRange(cx, cy, r, x, y)`, `bresenham(x0, y0, x1, y1, cb)` returning visited tiles; `randInt(lo, hi)` with seeded RNG.
- Seeded RNG: simple LCG or mulberry32 seeded from `crypto.getRandomValues` default, optional URL param seed.

## Map Generation (`map.ts`)
- Functions: `initMap()`, `digLevel()`, `hasSpace()`, `digRoom()`, `digPath()` mirroring C flow; configs `ROOM_RADIUS`, `PATHS`.
- Workflow: fill walls, clear view/colors, call `digLevel`, place single upstairs tile on random floor.
- Represent walls/floors as enum and maintain `tileColorM` for blood tinting.

## Entities and AI (`entities.ts`)
- Globals: `entL: Ent[]`, `entM` occupancy grid.
- Init: `initEnts(level)` clears occupancy, places player at center or random floor, spawns arachnids avoiding collisions and walls; player stats reset at level 1.
- FOV: `fov(player)` uses `bresenham`/`los` with `FOV_RADIUS` scaled by battery; updates `viewM` to `IN_SIGHT`/`SEEN`.
- Movement/combat: `moveTo(ent, nx, ny)` blocks walls; if target occupied by living entity, resolves damage similar to C (player/enemy only), handles death and blood tint.
- AI: `moveEnemy(e)` wakes on LOS within `FOV_RADIUS`; picks adjacent tiles sorted by distance to player; random wander when asleep.

## Items (`items.ts`)
- Globals: `itemL: Item[]`, `itemM` placement grid.
- Init: `initItems()` clears maps, scatters items on floors avoiding stairs/entities/items; type order matches C (med, air, battery, coins).
- Use: `useItem(player)` auto-consumes item on player tile; heals/airs/batteries up to caps; increments coins; sets `used` flag and clears placement map; triggers audio stub.

## Rendering and UI (`io.ts`)
- Canvas-based map render (2D context). Colors mapped from curses palette to CSS strings.
- Keep glyph-like feel by drawing monospace characters on canvas grid (cell size ~16px) instead of tiles.
- HUD/sidebar as DOM: HP/air/battery bars, coin count, level indicators, item legend, messages list.
- Message log: simple array; render last N lines into a fixed-height div; `addMessage(text)` helper.
- `drawScreen()` paints map: fog/seen colors, corpses/blood, items hidden in unseen, living entities only when visible.
- `printInfo()` updates HUD elements; called each turn.

## Input Handling (`io.ts`)
- Listen to `keydown`; normalize arrows/numpad/hjkl/vi keys to move/wait/help/quit/toggle light/stairs similar to `md_readchar` mapping subset.
- Maintain a mapping `KeyMap` from event.code to intents; ignore key repeat if needed.
- Modal help overlay triggered via `?`/`F1`; pause input when open.

## Game Loop (`main.ts`)
- Startup: seed RNG, `initCanvas()`, `initUI()`, `initMap()`, `initEnts(level)`, `initItems()`, initial `fov()`/`drawScreen()`/`printInfo()`, show help once.
- Turn loop driven by `async` loop awaiting player input Promise:
  1) `playerAction()` waits for valid input mapped to move/wait/toggle/help/quit.
  2) `useItem()` on player tile.
  3) Enemy phase: iterate arachnids, gate by speed, call `moveEnemy()`; resolve combat in `moveTo`.
  4) Decay FOV tiles; drain battery if light on; decrement air; check loss conditions.
  5) Recompute `fov()`, `drawScreen()`, `printInfo()`.
- End conditions: `youWon()` when `level > LAST_LEVEL`; `youLost(reason)` on death or quit; both call `endGame()`.

## Audio Stubs (`audio.ts`)
- Functions: `playMusic(name: string)`, `stopMusic()`, `playSound(name: string)`; empty bodies now, hookable later.
- Calls placed where SDL versions trigger sounds (hits, alerts, pickups, music start/stop).

## Persistence
- Highscores optional later: localStorage entry storing score list; keep interface similar to `scores.dat` (name, score, date). For now, stub `saveScore()`/`loadScores()` returning empty list.

## Rendering Assets
- No external assets required; colors defined in CSS variables. Optional future sprites can replace glyph rendering.

## HTML/CSS Layout
- HTML: `<canvas id="map">`, `<section id="sidebar">` for HUD, `<div id="messages">`, `<div id="help-modal">` hidden by default.
- CSS: flex row with canvas and sidebar; monospace font; color variables matching curses palette; responsive (canvas scales via CSS while keeping fixed logical grid size).

## TypeScript Project Setup
- `tsconfig.json` with `module` ES2020, `target` ES2020, `strict` true, `outDir` `./dist`, `rootDir` `./src-ts`.
- Source layout mirrors C files in `src-ts/`.
- Simple `npm` script: `tsc --watch` for dev; static file server (e.g., `python -m http.server`) for testing.
- No `any` types; always declare concrete interfaces/enums/tuples so surfaces stay typed.

## Pragmatic Test Harness (incremental)
- Add `tests/harness.ts` (compiled to `dist/tests/harness.js`) with tiny `assert(cond, msg)` helper; run via `npm test` script `node dist/tests/harness.js`. Keep pure logic—no DOM/jsdom.
- Use seeded RNG to make checks deterministic (`seed=1234` default, override via env/arg). Export `setSeed` from `utils.ts` to share seeding.
- Stage the checks to match the implementation order:
  1) Utils: validate `dist2`, `inRange`, and `bresenham` hits expected coordinates for a few pairs; assert `randInt` stays within bounds and is repeatable with the same seed.
  2) Map: after `digLevel`, assert at least one stair, stair sits on a floor, and there is a path of floors from center to stair (simple flood-fill on `tileM`).
  3) Entities: after `initEnts`, assert player on floor, no overlapping `entM` entries, and enemy count matches config; `moveTo` blocks walls and swaps occupancy correctly when combat resolves death.
  4) Items: after `initItems`, assert no overlaps with entities/stairs, counts per type match expectations, and `useItem` flips `used` and clears `itemM`.
  5) FOV/LOS: seed a small synthetic map to assert that `fov` only marks tiles within `FOV_RADIUS` and respects walls by comparing two known layouts.
- Keep harness fast (<1s) and runnable after each milestone; fail fast with terse messages. Prefer a couple of representative cases over exhaustive suites.
- For rendering/input, rely on manual smoke runs (canvas + keyboard) but keep a saved harness seed to reproduce logic regressions before debugging UI.

## Testing/Verification
- Manual smoke tests: movement, FOV reveal, combat resolution, item pickup, level transitions, loss conditions (HP/air/battery).
- Add small unit tests for utilities (`bresenham`, `los`, RNG determinism) using `vitest` optional; otherwise keep manual.

## Parity Checklist vs C
- Map: room/path carving logic and parameters; single stair placement.
- Entities: counts, stats, FOV radius scaling, wake logic, speed gate, combat damage ranges.
- Items: spawn counts/types/order and usage caps.
- UI: message texts, info bars, help modal content matching `show_help()` strings.
- Audio: stub calls present in same spots as SDL code.

## Incremental Implementation Order
1) Set up TypeScript project + HTML/CSS skeleton and canvas grid rendering of empty map.
2) Port utilities (`dist2`, `inRange`, `bresenham`, RNG).
3) Port map generation; visualize rooms/paths on canvas.
4) Port entities init and player movement + collision walls.
5) Implement FOV/LOS and fog rendering.
6) Add enemy AI + combat; blood tinting.
7) Add items spawn/use.
8) HUD/message log/end-game handling.
9) Add help modal and quit/win flows.
10) Wire audio stubs; add polish (animations optional).
