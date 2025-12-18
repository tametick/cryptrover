// CryptRover Web - Utility functions

// Seeded RNG state
let rngState = 0;

// Set RNG seed
export function setSeed(seed: number): void {
  rngState = seed >>> 0;
}

// Initialize RNG with random seed from crypto
export function initRng(): number {
  const arr = new Uint32Array(1);
  crypto.getRandomValues(arr);
  const seed = arr[0];
  setSeed(seed);
  return seed;
}

// Mulberry32 PRNG - returns float in [0, 1)
function mulberry32(): number {
  rngState += 0x6D2B79F5;
  let t = rngState;
  t = Math.imul(t ^ (t >>> 15), t | 1);
  t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
  return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
}

// Random integer in [lo, hi] inclusive
export function randInt(lo: number, hi: number): number {
  return Math.floor(mulberry32() * (hi - lo + 1)) + lo;
}

// Math helpers
export function min(a: number, b: number): number {
  return a < b ? a : b;
}

export function max(a: number, b: number): number {
  return a > b ? a : b;
}

export function swap(arr: number[], i: number, j: number): void {
  const tmp = arr[i];
  arr[i] = arr[j];
  arr[j] = tmp;
}

// Squared distance (no sqrt for efficiency)
export function dist2(ax: number, ay: number, bx: number, by: number): number {
  const dx = bx - ax;
  const dy = by - ay;
  return dx * dx + dy * dy;
}

// Check if point (x, y) is within radius r of center (cx, cy)
export function inRange(cx: number, cy: number, r: number, x: number, y: number): boolean {
  return dist2(cx, cy, x, y) <= r * r;
}

// Bresenham's line algorithm - returns array of points visited
export function bresenham(
  x0: number,
  y0: number,
  x1: number,
  y1: number
): Array<{ x: number; y: number }> {
  const points: Array<{ x: number; y: number }> = [];

  const dx = Math.abs(x1 - x0);
  const dy = Math.abs(y1 - y0);
  const sx = x0 < x1 ? 1 : -1;
  const sy = y0 < y1 ? 1 : -1;
  let err = dx - dy;

  let x = x0;
  let y = y0;

  while (true) {
    points.push({ x, y });

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

  return points;
}

// Absolute value
export function abs(n: number): number {
  return n < 0 ? -n : n;
}

// Clamp value between min and max
export function clamp(value: number, minVal: number, maxVal: number): number {
  return min(max(value, minVal), maxVal);
}
