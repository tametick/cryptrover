# CryptRover: Code Tour

## Build and Run
- Standard GNU autotools flow: `./configure && make` builds the curses binary `cr`; SDL sound can be toggled via configure flags noted in README/Makefile (targets add `-std=c99`, optimize by default, `DBG` enables `-O0 -g -Wall -Wextra -pedantic`).
- Runtime assets: optional `media/*.wav`/`*.ogg` for SDL sound/music and a `scores.dat` created/appended on game end.

## Data Shapes (shared globals)
- Map grids: `tile_m[Y_][X_]`, `tile_color_m`, `view_m` in [src/main.c](src/main.c) and defined in [src/map.h](src/map.h).
- Entities: `ent_l[ENTS_]` (player at index 0) and occupancy map `ent_m[Y_][X_]` declared in [src/entities.h](src/entities.h) and defined in [src/main.c](src/main.c).
- Items: `item_l[ITEMS_]` and placement map `item_m[Y_][X_]` declared in [src/items.h](src/items.h) and defined in [src/main.c](src/main.c).

## Core Loop ([src/main.c](src/main.c))
- Initializes RNG, curses (and SDL audio if enabled), then calls `init_map()`, `init_ents(level)`, and `init_items()`.
- Sets up initial FOV/draw, info sidebar, message window, shows help modal, then enters the main turn loop:
  1) `player_action()` repeatedly reads input until a valid move/wait/toggle/stair/help/quit.
  2) `use_item()` auto-applies an item on the player tile.
  3) Enemies advance: for each arachnid, if alive and its speed gate passes, `move_enemy()` chases or wanders; combat resolves via `move_to()`.
  4) Old FOV tiles decay from `IN_SIGHT` to `SEEN`; battery drains if flashlight on; air decrements with low-air warning; suffocation or 0 HP triggers loss.
  5) Recompute FOV (`fov()`), redraw (`draw_screen()` + `print_info()`).
- End conditions: `you_won()` when surpassing `LAST_LEVEL`; `you_lost()` on death or quit, both funnel through `end_game()` to save score and show highscores.

## Map Generation ([src/map.c](src/map.c), [src/map.h](src/map.h))
- Constants: `Y_`=24, `X_`=48, tiles `WALL`/`FLOOR`/`NEXT_LEVEL`, generation params `ROOM_RADIUS`=2, `PATHS`=5, `LAST_LEVEL`=12.
- Workflow: `init_map()` fills walls, clears view/colors, calls `dig_level()`, then plants a single `<` stair on a random floor.
- `dig_level()` repeatedly carves rooms from the center outward: picks radial/square rooms within bounds (`has_space()`); digs rooms (`dig_room()`), connects with Bresenham corridors (`dig_path()`), and branches up to PATHS times until attempts fail.

## Entities and AI ([src/entities.c](src/entities.c), [src/entities.h](src/entities.h))
- Struct `ent_t`: id, position, vitals (hp, air, battery, coins), speed, glyph/color, flags `awake`, `light_on`.
- Init: `init_ents(level)` clears occupancy map, places player then 11 arachnids randomly avoiding walls/overlap/near-player. Player stats set only on level 1.
- Vision: `fov()` marks `view_m` tiles within flashlight radius (scaled by battery/`PLAYER_BATTERY`) if line-of-sight passes through `los()`.
- Movement/combat: `move_to()` blocks walls, otherwise moves; if destination occupied by a living entity, resolves a hit (player↔enemy only), handles death cleanup and blood tint, returns success to keep turn advancing.
- AI: `move_enemy()` wakes on LOS within `FOV_RADIUS`, sorts adjacent tiles by distance (`compare_tiles()`), tries moves in order; sleeping foes move randomly.

## Items and Pickup ([src/items.c](src/items.c), [src/items.h](src/items.h))
- Struct `item_t`: position, glyph/type, color, `used` flag.
- Init: `init_items()` clears placement map, scatters items on floor not on stairs/entities/items; assignment order sets type and color for med packs, air cans, batteries, then coins.
- Use: `use_item()` triggers when the player stands on an unused item—heals HP, restores air or battery up to caps, or increments coins; ignored if already full; plays sounds when SDL is active.

## Rendering and UI ([src/io.c](src/io.c), [src/io.h](src/io.h))
- Curses setup via `init_curses()`; panels/windows for help, highscores, message log. Color macros: `C_FOG`, `C_MED`, `C_AIR`, `C_BAT`, `C_COIN`, `C_LVL`.
- `draw_screen()` paints map tiles with fog status, corpses on seen/sight tiles, items (hidden in unseen), and living entities only when visible.
- `print_info()` renders sidebar bars (HP/air/battery), coin count, dungeon progress marks, and an item legend. `add_message()`/`init_message_win()` manage the scrollback window.
- `show_help()` modal documents controls; `show_highscore()` reads, sorts, and displays `scores.dat` entries.

## Input Normalization ([src/mdport.c](src/mdport.c), [src/mdport.h](src/mdport.h))
- `md_readchar()` translates diverse keypad/shift/ctrl/alt/ESC sequences across terminals (ncurses/PDCurses) into rogue-like movement keys, with timeout handling to fall back gracefully. `readchar()` in io.c simply delegates to this.

## Utilities ([src/utils.c](src/utils.c), [src/utils.h](src/utils.h))
- Helpers `min/max/swap`, squared-distance `dist()`, circle test `in_range()`, and Bresenham `los()` that can also apply a callback per tile (used by map digging and FOV).

## Audio (optional SDL paths)
- SDL blocks gated by `__SDL__`: sound effects for bites/punches/grunts/alerts and item pickups in entities/items; background music in main; all gracefully skipped in curses-only builds.

## Key Constants (quick lookup)
- Map: `Y_`, `X_`, `ROOM_RADIUS`, `PATHS`, `LAST_LEVEL`, tile glyphs in [src/map.h](src/map.h).
- Entities: counts/stats/FOV and glyphs in [src/entities.h](src/entities.h).
- Items: glyphs, charges, counts in [src/items.h](src/items.h).
- UI/Input: color macros and key codes in [src/io.h](src/io.h).
