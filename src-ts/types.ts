// CryptRover Web - Type definitions

import { UNSEEN, SEEN, IN_SIGHT } from './constants.js';

// View state type
export type ViewState = typeof UNSEEN | typeof SEEN | typeof IN_SIGHT;

// Entity interface (player and enemies)
export interface Ent {
  id: number;
  x: number;
  y: number;
  hp: number;
  air: number;
  battery: number;
  coins: number;
  speed: number;
  awake: boolean;
  lightOn: boolean;
  alive: boolean;
  glyph: string;
  color: string;
}

// Item interface
export interface Item {
  x: number;
  y: number;
  glyph: string;
  type: string;
  color: string;
  used: boolean;
}

// Tile interface
export interface Tile {
  y: number;
  x: number;
  type: string;
}

// Grid helper to create 2D arrays
export function makeGrid<T>(height: number, width: number, init: () => T): T[][] {
  const grid: T[][] = [];
  for (let y = 0; y < height; y++) {
    grid[y] = [];
    for (let x = 0; x < width; x++) {
      grid[y][x] = init();
    }
  }
  return grid;
}

// Point type for coordinates
export interface Point {
  x: number;
  y: number;
}

// Direction vectors for 8-directional movement
export const DIRECTIONS: readonly Point[] = [
  { x: 0, y: -1 },  // up
  { x: 0, y: 1 },   // down
  { x: -1, y: 0 },  // left
  { x: 1, y: 0 },   // right
  { x: -1, y: -1 }, // up-left
  { x: 1, y: -1 },  // up-right
  { x: -1, y: 1 },  // down-left
  { x: 1, y: 1 },   // down-right
] as const;
