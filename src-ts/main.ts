// CryptRover Web - Main entry point

import { X_, Y_, CELL_SIZE, Colors, WALL, FLOOR, NEXT_LEVEL } from './constants.js';
import { initMap, tileM } from './map.js';
import { initRng } from './utils.js';

// Canvas and context
let canvas: HTMLCanvasElement;
let ctx: CanvasRenderingContext2D;

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
  ctx.font = `bold ${CELL_SIZE - 2}px monospace`;
  ctx.textBaseline = 'top';
}

// Draw the map grid
export function drawScreen(): void {
  // Clear canvas
  ctx.fillStyle = Colors.black;
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Draw each tile
  for (let y = 0; y < Y_; y++) {
    for (let x = 0; x < X_; x++) {
      const tile = tileM[y][x];
      const glyph = tile.type;
      const px = x * CELL_SIZE;
      const py = y * CELL_SIZE;

      // Choose color based on tile type
      let color = Colors.darkGray;
      if (glyph === FLOOR) {
        color = Colors.floor;
      } else if (glyph === WALL) {
        color = Colors.wall;
      } else if (glyph === NEXT_LEVEL) {
        color = Colors.stairs;
      }

      ctx.fillStyle = color;
      ctx.fillText(glyph, px + 2, py + 1);
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

  // Initialize map (generates dungeon)
  initMap();

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
