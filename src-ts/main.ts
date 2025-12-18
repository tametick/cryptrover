// CryptRover Web - Main entry point

import { X_, Y_, CELL_SIZE, Colors, WALL, FLOOR } from './constants.js';
import { makeGrid } from './types.js';
import { initRng } from './utils.js';

// Canvas and context
let canvas: HTMLCanvasElement;
let ctx: CanvasRenderingContext2D;

// Global state (exported for use by other modules in later steps)
export let tileM: string[][];
export let tileColorM: string[][];
export let viewM: number[][];

// Initialize canvas
function initCanvas(): void {
  canvas = document.getElementById('map') as HTMLCanvasElement;
  const context = canvas.getContext('2d');
  if (!context) {
    throw new Error('Failed to get canvas 2D context');
  }
  ctx = context;

  // Set canvas size
  canvas.width = X_ * CELL_SIZE;
  canvas.height = Y_ * CELL_SIZE;

  // Configure context for crisp pixel rendering
  ctx.imageSmoothingEnabled = false;
  ctx.font = `${CELL_SIZE}px monospace`;
  ctx.textBaseline = 'top';
}

// Initialize grids
function initGrids(): void {
  tileM = makeGrid(Y_, X_, () => WALL);
  tileColorM = makeGrid(Y_, X_, () => Colors.wall);
  viewM = makeGrid(Y_, X_, () => 0);
}

// Draw the map grid (placeholder - shows empty map with walls)
function drawScreen(): void {
  // Clear canvas
  ctx.fillStyle = Colors.black;
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Draw each tile
  for (let y = 0; y < Y_; y++) {
    for (let x = 0; x < X_; x++) {
      const tile = tileM[y][x];
      const px = x * CELL_SIZE;
      const py = y * CELL_SIZE;

      // Choose color based on tile type
      let color = Colors.wall;
      if (tile === FLOOR) {
        color = Colors.floor;
      } else if (tile === WALL) {
        color = Colors.wall;
      }

      ctx.fillStyle = color;
      ctx.fillText(tile, px, py);
    }
  }
}

// Main initialization
function init(): void {
  console.log('CryptRover Web initializing...');

  // Initialize RNG
  const seed = initRng();
  console.log(`RNG seed: ${seed}`);

  // Initialize canvas
  initCanvas();

  // Initialize grids
  initGrids();

  // Initial draw
  drawScreen();

  console.log('CryptRover Web ready!');
  console.log(`Map size: ${X_}x${Y_}, Cell size: ${CELL_SIZE}px`);
}

// Start when DOM is ready
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', init);
} else {
  init();
}
