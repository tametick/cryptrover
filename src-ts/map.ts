// CryptRover Web - Map generation

import {
  Y_, X_, WALL, FLOOR, NEXT_LEVEL, ROOM_RADIUS, PATHS, UNSEEN,
  Colors
} from './constants.js';
import { makeGrid, type Tile } from './types.js';
import { randInt, inRange, abs } from './utils.js';

// Global map state
export let tileM: Tile[][];
export let tileColorM: string[][];
export let viewM: number[][];

// Initialize grids
function initGrids(): void {
  tileM = makeGrid(Y_, X_, () => ({ y: 0, x: 0, type: WALL }));
  tileColorM = makeGrid(Y_, X_, () => Colors.wall);
  viewM = makeGrid(Y_, X_, () => UNSEEN);

  // Set coordinates
  for (let y = 0; y < Y_; y++) {
    for (let x = 0; x < X_; x++) {
      tileM[y][x].y = y;
      tileM[y][x].x = x;
    }
  }
}

// Check if there is enough free space for a room
function hasSpace(y: number, x: number, radius: number): boolean {
  if (y - radius < 1 || x - radius < 1 || y + radius >= Y_ - 1 || x + radius >= X_ - 1) {
    return false;
  }

  for (let yy = y - radius - 1; yy <= y + radius + 1; yy++) {
    for (let xx = x - radius - 1; xx <= x + radius + 1; xx++) {
      if (tileM[yy][xx].type === FLOOR) {
        return false;
      }
    }
  }
  return true;
}

// Dig a single tile (make it floor)
function digTile(y: number, x: number): void {
  if (y >= 0 && y < Y_ && x >= 0 && x < X_) {
    tileM[y][x].type = FLOOR;
  }
}

// Line of sight / Bresenham with callback - ported from C
function los(
  y0: number,
  x0: number,
  y1: number,
  x1: number,
  apply: ((y: number, x: number) => void) | null
): boolean {
  // Bresenham's line algorithm
  const steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    [x0, y0] = [y0, x0];
    [x1, y1] = [y1, x1];
  }
  if (x0 > x1) {
    [x0, x1] = [x1, x0];
    [y0, y1] = [y1, y0];
  }

  let errNum = 0.0;
  let y = y0;
  const dx = x1 - x0;
  const dy = abs(y1 - y0);
  const yStep = y1 > y0 ? 1 : -1;

  for (let x = x0; x <= x1; x++) {
    if (x > x0 && x < x1) {
      const ty = steep ? x : y;
      const tx = steep ? y : x;
      if (apply) {
        apply(ty, tx);
      }
    }

    if (dx !== 0) {
      errNum += dy / dx;
    }
    if (errNum > 0.5) {
      y += yStep;
      errNum -= 1;
    }
  }
  return true;
}

// Dig a path between two points
function digPath(y0: number, x0: number, y1: number, x1: number): void {
  los(y0, x0, y1, x1, digTile);
}

// Dig a room at position with given radius
function digRoom(y: number, x: number, radius: number, radial: boolean): boolean {
  if (!hasSpace(y, x, radius)) {
    return false;
  }

  for (let yy = y - radius; yy <= y + radius; yy++) {
    for (let xx = x - radius; xx <= x + radius; xx++) {
      if ((radial && inRange(y, x, radius, yy, xx)) || !radial) {
        tileM[yy][xx].type = FLOOR;
      }
    }
  }

  return true;
}

// Dig the level - generate rooms and corridors
function digLevel(): void {
  let newRy = 0;
  let newRx = 0;
  let radius = 1 + randInt(0, ROOM_RADIUS - 1);
  let radial = randInt(0, 1) === 1;
  let outerTries = 0;
  const MAX_OUTER_TRIES = 100;

  while (outerTries++ < MAX_OUTER_TRIES) {
    // Continue digging from the last new room or
    // dig the first room in the middle of the level
    const ry = newRy !== 0 ? newRy : Math.floor(Y_ / 2);
    const rx = newRx !== 0 ? newRx : Math.floor(X_ / 2);

    if ((newRx !== 0 && newRy !== 0) || digRoom(ry, rx, radius, radial)) {
      const paths = 1 + randInt(0, PATHS - 1);

      for (let p = 0; p < paths; p++) {
        let tries = 0;

        // Try to find an empty space and dig a room there
        while (tries++ < 10000) {
          newRy = ry + randInt(0, 8 * radius - 1) - 4 * radius;
          newRx = rx + randInt(0, 8 * radius - 1) - 4 * radius;
          radius = 1 + randInt(0, ROOM_RADIUS - 1);
          radial = randInt(0, 1) === 1;

          if (digRoom(newRy, newRx, radius, radial)) {
            break;
          }
        }

        if (tries > 10000) {
          return;
        }

        // Connect the old room to the new room
        digPath(ry, rx, newRy, newRx);
      }
    }
  }
}

// Initialize the map
export function initMap(): void {
  initGrids();
  digLevel();

  // Place entry to next level on a random floor tile
  let ny: number;
  let nx: number;
  do {
    ny = randInt(0, Y_ - 1);
    nx = randInt(0, X_ - 1);
  } while (tileM[ny][nx].type === WALL);

  tileM[ny][nx].type = NEXT_LEVEL;
}

// Get a random floor tile position (useful for spawning)
export function getRandomFloor(): { y: number; x: number } {
  let y: number;
  let x: number;
  do {
    y = randInt(0, Y_ - 1);
    x = randInt(0, X_ - 1);
  } while (tileM[y][x].type !== FLOOR);
  return { y, x };
}

// Check if a position is walkable (floor or stairs)
export function isWalkable(y: number, x: number): boolean {
  if (y < 0 || y >= Y_ || x < 0 || x >= X_) {
    return false;
  }
  const type = tileM[y][x].type;
  return type === FLOOR || type === NEXT_LEVEL;
}

// Expose los for FOV calculations later
export { los };
