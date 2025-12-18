# CryptRover: Game Design Guide

## Premise and Goal
- Descend/ascend through a procedurally dug crypt of connected rooms and corridors to reach the staircase on each floor and climb until beyond level 12 to win ([src/main.c](src/main.c), [src/map.c](src/map.c)).
- Lose when HP drops to 0, air reaches 0 (suffocation), or you quit; the run then saves a score entry showing gold, depth, and remaining vitals ([src/main.c](src/main.c), [src/io.c](src/io.c)).

## Map and Progression
- Each level is a 24x48 grid of walls `#`, floors `.`, and one up-stair `<` placed on a random floor tile ([src/map.h](src/map.h), [src/map.c](src/map.c)).
- Rooms (square or radial) are carved if empty space exists; up to five corridor branches connect to new rooms until generation stalls, ensuring a fully connected dungeon ([src/map.c](src/map.c)).
- Field of view is limited; tiles move from unseen → seen (fog) → in-sight, and corpses/items fade to fog when outside sight ([src/entities.c](src/entities.c), [src/io.c](src/io.c)).

## Player
- Avatar glyph `@`, starts on level 1 with HP 18, air 105, battery 180, flashlight on, zero coins ([src/entities.c](src/entities.c), [src/entities.h](src/entities.h), [src/items.h](src/items.h)).
- Acts every turn (speed 0 special-case); FOV radius 5 scaled by remaining battery when flashlight is on ([src/entities.c](src/entities.c)).
- Standing on items auto-uses them; standing on `<` and using climb input moves to the next level ([src/main.c](src/main.c), [src/items.c](src/items.c)).

## Controls (all repeatable with vi, WASD, or numpad clusters)
- Move/attack: 8-directional using `h j k l y u b n`, `wasd` + diagonals `q e z c`, or numpad `1-9`.
- Wait: `.` or `s` or `5`.
- Toggle flashlight: `f` (affects FOV and battery drain).
- Climb stair: `<` or `,` when on the stair tile.
- Help: `?` shows control legend; Quit: `ESC` or `Ctrl+C` ([src/main.c](src/main.c), [src/io.c](src/io.c), [src/mdport.c](src/mdport.c)).

## Enemies
- Arachnids `a`: 11 per level, HP 2, speed 3 (act only when turn % speed != 0), red color, start asleep ([src/entities.c](src/entities.c), [src/entities.h](src/entities.h)).
- Wake when player is within the nominal FOV radius and line-of-sight; awake enemies chase by picking adjacent tiles closest to the player, otherwise wander randomly ([src/entities.c](src/entities.c)).
- Combat: moving into an occupied tile triggers a hit; arachnid bites lower HP and warn at low HP, player hits can kill and leave blood tint, death message if player killed ([src/entities.c](src/entities.c)).

## Items and Resources
- Items placed on empty floor away from stairs/entities: 3 med packs `+` (heal +3 HP), 4 air canisters `*` (+21 air), 3 batteries `!` (+36 battery), 5 coins `$` (+1 gold) ([src/items.c](src/items.c), [src/items.h](src/items.h)).
- Auto-use on pickup if not already full; coins always collected. Sounds play when SDL is enabled ([src/items.c](src/items.c)).
- Resources:
  - HP: lost to enemy bites; warning at medium HP; death at 0 ([src/entities.c](src/entities.c), [src/io.c](src/io.c)).
  - Air: decreases every turn; warning below the `AIR_CHARGE` threshold; suffocation at 0 ([src/main.c](src/main.c)).
  - Battery: drains only while flashlight is on; FOV shrinks with charge ([src/entities.c](src/entities.c), [src/main.c](src/main.c)).
  - Gold: score tally, persists to scoreboard on exit ([src/main.c](src/main.c), [src/io.c](src/io.c)).

## Turn Structure and Feedback
- Turn steps: accept a valid player action → auto-use item underfoot → move each living arachnid if their speed gate passes → age FOV to fog → drain battery/air and emit low-air alerts → recompute FOV → redraw map + HUD ([src/main.c](src/main.c)).
- UI: left pane is the map with fog; right pane shows bars for HP/air/battery, coin count, dungeon level progress, and an item legend; bottom/right message window scrolls events and warnings ([src/io.c](src/io.c)).
- Sound (optional SDL): looping music plus bite/punch/grunt/alert/money/clunk effects; otherwise silent in curses-only builds ([src/main.c](src/main.c), [src/entities.c](src/entities.c), [src/items.c](src/items.c)).

## Winning, Losing, and Scores
- Win by stepping on `<` and advancing past level 12; loss via death or quit path triggers score save and high-score display ([src/main.c](src/main.c), [src/io.c](src/io.c)).
- Scores saved to `scores.dat` and displayed sorted with the current run highlighted ([src/main.c](src/main.c), [src/io.c](src/io.c)).
