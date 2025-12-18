// CryptRover Web - Entity management

import {
  Y_, X_, WALL, ENTS_, FOV_RADIUS,
  PLAYER_HP, PLAYER_AIR, PLAYER_BATTERY, MED_CHARGE,
  PLAYER, ARACHNID, Colors, IN_SIGHT
} from './constants.js';
import { type Ent, makeGrid } from './types.js';
import { tileM, tileColorM, viewM } from './map.js';
import { randInt, dist2, inRange, max, min } from './utils.js';
import { addMessage } from './io.js';
import { playSound } from './audio.js';

// Global entity state
export let entL: Ent[] = [];
export let entM: (Ent | null)[][] = [];

// Check if position is close to player (within FOV_RADIUS - 1)
function closeToPlayer(y: number, x: number): boolean {
  const player = entL[0];
  return inRange(player.y, player.x, FOV_RADIUS - 1, y, x);
}

// Initialize entities for a level
export function initEnts(level: number): void {
  // Clear occupancy map
  entM = makeGrid(Y_, X_, () => null);

  // Create entity list if first time
  if (entL.length === 0) {
    for (let e = 0; e < ENTS_; e++) {
      entL.push({
        id: e,
        x: 0,
        y: 0,
        hp: 0,
        air: 0,
        battery: 0,
        coins: 0,
        speed: 0,
        awake: false,
        lightOn: true,
        alive: true,
        glyph: PLAYER,
        color: Colors.player,
      });
    }
  }

  // Place entities
  for (let e = 0; e < ENTS_; e++) {
    const ent = entL[e];
    ent.id = e;
    ent.awake = false;
    ent.alive = true;

    // Find valid position
    let y: number;
    let x: number;
    do {
      y = randInt(0, Y_ - 1);
      x = randInt(0, X_ - 1);
    } while (
      tileM[y][x].type === WALL ||
      entM[y][x] !== null ||
      (e > 0 && closeToPlayer(y, x))
    );

    ent.y = y;
    ent.x = x;

    if (e > 0) {
      // Enemy (arachnid)
      ent.hp = 2;
      ent.air = 1;
      ent.speed = 3;
      ent.battery = 1;
      ent.coins = 0;
      ent.lightOn = true;
      ent.glyph = ARACHNID;
      ent.color = Colors.enemy;
    }

    entM[y][x] = ent;
  }

  // Player initial attributes (only on level 1)
  const player = entL[0];
  if (level === 1) {
    player.hp = PLAYER_HP;
    player.air = PLAYER_AIR;
    player.speed = 0; // Special case: move every turn
    player.battery = PLAYER_BATTERY;
    player.coins = 0;
    player.lightOn = true;
    player.glyph = PLAYER;
    player.color = Colors.player;
  }
}

// Get player entity
export function getPlayer(): Ent {
  return entL[0];
}

// Field of view calculation
export function fov(y: number, x: number, radius: number): void {
  const player = entL[0];
  const effectiveRadius = player.lightOn
    ? Math.floor((radius * player.battery) / PLAYER_BATTERY)
    : 0;

  for (let yy = max(y - effectiveRadius, 0); yy <= min(y + effectiveRadius, Y_ - 1); yy++) {
    for (let xx = max(x - effectiveRadius, 0); xx <= min(x + effectiveRadius, X_ - 1); xx++) {
      if (losCheck(y, x, yy, xx)) {
        viewM[yy][xx] = IN_SIGHT;
      }
    }
  }
}

// Line of sight check (no callback, just returns if clear)
function losCheck(y0: number, x0: number, y1: number, x1: number): boolean {
  // Simple Bresenham check for walls
  const dx = Math.abs(x1 - x0);
  const dy = Math.abs(y1 - y0);
  const sx = x0 < x1 ? 1 : -1;
  const sy = y0 < y1 ? 1 : -1;
  let err = dx - dy;

  let x = x0;
  let y = y0;

  while (true) {
    // Don't check start and end points
    if ((x !== x0 || y !== y0) && (x !== x1 || y !== y1)) {
      if (y >= 0 && y < Y_ && x >= 0 && x < X_) {
        if (tileM[y][x].type === WALL) {
          return false;
        }
      }
    }

    if (x === x1 && y === y1) break;

    const e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }

  return true;
}

// Compare tiles by distance to player (for sorting)
function compareTilesByDistToPlayer(
  t1: { y: number; x: number },
  t2: { y: number; x: number }
): number {
  const player = entL[0];
  const d1 = dist2(t1.y, t1.x, player.y, player.x);
  const d2 = dist2(t2.y, t2.x, player.y, player.x);
  return d1 - d2;
}

// Move entity - returns true if turn was consumed
export function moveTo(ent: Ent, dy: number, dx: number): boolean {
  const newY = ent.y + dy;
  const newX = ent.x + dx;

  // Bounds check
  if (newY < 0 || newY >= Y_ || newX < 0 || newX >= X_) {
    return false;
  }

  // Don't move into walls
  if (tileM[newY][newX].type === WALL) {
    if (ent.id === 0) {
      addMessage('There is a wall in the way!', '');
    }
    return false;
  }

  // If destination has an entity
  const target = entM[newY][newX];
  if (target !== null && target.alive) {
    // Combat! Only player attacks enemies or enemies attack player
    if (ent.id === 0 || target.id === 0) {
      target.hp--;

      if (ent.id > 0) {
        // Enemy attacks player
        addMessage('The arachnid bites you.', 'danger');
        playSound('bite');
        playSound('grunt');
        if (target.hp <= MED_CHARGE && target.hp > 0) {
          addMessage('DANGER - LOW HITPOINTS.', 'danger');
          playSound('alert');
        }
      } else {
        // Player attacks enemy
        addMessage('You hit the arachnid.', '');
        playSound('punch');
      }

      // Check if target died
      if (target.hp < 1) {
        target.alive = false;
        entM[target.y][target.x] = null;

        if (ent.id === 0) {
          addMessage('You kill the arachnid!', 'success');
          // Blood splatter
          tileColorM[newY][newX] = Colors.blood;
          for (let yy = target.y - 1; yy <= target.y + 1; yy++) {
            for (let xx = target.x - 1; xx <= target.x + 1; xx++) {
              if (yy >= 0 && yy < Y_ && xx >= 0 && xx < X_) {
                if (randInt(0, 3) !== 0) {
                  tileColorM[yy][xx] = Colors.blood;
                }
              }
            }
          }
        } else {
          addMessage('The arachnid kills you!', 'danger');
        }
      }

      return true; // Turn consumed even if just attacked
    }
    return false; // Enemies don't attack each other
  }

  // Move to empty tile
  entM[ent.y][ent.x] = null;
  ent.y = newY;
  ent.x = newX;
  entM[newY][newX] = ent;
  return true;
}

// Move enemy AI
export function moveEnemy(enemy: Ent, player: Ent): void {
  if (!enemy.alive) return;

  // Check if enemy should wake up (sees player)
  if (
    enemy.awake ||
    (Math.abs(enemy.y - player.y) <= FOV_RADIUS &&
      Math.abs(enemy.x - player.x) <= FOV_RADIUS &&
      losCheck(enemy.y, enemy.x, player.y, player.x))
  ) {
    if (!enemy.awake) {
      enemy.awake = true;
    }

    // Get adjacent tiles and sort by distance to player
    const adjTiles: Array<{ y: number; x: number }> = [];
    for (let y = enemy.y - 1; y <= enemy.y + 1; y++) {
      for (let x = enemy.x - 1; x <= enemy.x + 1; x++) {
        if (y >= 0 && y < Y_ && x >= 0 && x < X_) {
          adjTiles.push({ y, x });
        }
      }
    }
    adjTiles.sort(compareTilesByDistToPlayer);

    // Try to move to closest tile
    for (const tile of adjTiles) {
      const dy = tile.y - enemy.y;
      const dx = tile.x - enemy.x;
      if (moveTo(enemy, dy, dx)) {
        break;
      }
    }
  } else {
    // Sleeping enemies move randomly
    const dy = randInt(-1, 1);
    const dx = randInt(-1, 1);
    moveTo(enemy, dy, dx);
  }
}

// Process all enemy moves for a turn
export function moveAllEnemies(turn: number): void {
  const player = entL[0];
  for (let e = 1; e < ENTS_; e++) {
    const enemy = entL[e];
    // Speed gate: enemies with speed N move every N turns
    if (enemy.alive && enemy.speed && turn % enemy.speed === 0) {
      moveEnemy(enemy, player);
      // Check if player died
      if (player.hp < 1) {
        return;
      }
    }
  }
}

// Check if player is alive
export function isPlayerAlive(): boolean {
  return entL[0].hp > 0 && entL[0].air > 0;
}
