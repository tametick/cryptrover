// CryptRover Web - Main entry point

import {
  X_, Y_, CELL_SIZE, Colors, WALL, FLOOR, NEXT_LEVEL,
  PLAYER_HP, PLAYER_AIR, PLAYER_BATTERY, LAST_LEVEL
} from './constants.js';
import { initMap, tileM } from './map.js';
import { initRng } from './utils.js';
import {
  initInput, waitForInput, showHelp,
  updateHUD, addMessage, type InputAction
} from './io.js';

// Canvas and context
let canvas: HTMLCanvasElement;
let ctx: CanvasRenderingContext2D;

// Game state (temporary - will be moved to entities module)
let playerY = 12;
let playerX = 24;
let playerHp = PLAYER_HP;
let playerAir = PLAYER_AIR;
let playerBattery = PLAYER_BATTERY;
let playerCoins = 0;
let playerLightOn = true;
let level = 1;
let gameRunning = true;

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

// Find a walkable starting position
function findStartPosition(): { y: number; x: number } {
  // Start from center and search outward
  const centerY = Math.floor(Y_ / 2);
  const centerX = Math.floor(X_ / 2);

  for (let r = 0; r < Math.max(Y_, X_); r++) {
    for (let dy = -r; dy <= r; dy++) {
      for (let dx = -r; dx <= r; dx++) {
        const y = centerY + dy;
        const x = centerX + dx;
        if (y >= 0 && y < Y_ && x >= 0 && x < X_) {
          const type = tileM[y][x].type;
          if (type === FLOOR) {
            return { y, x };
          }
        }
      }
    }
  }
  return { y: centerY, x: centerX };
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

  // Draw player
  const px = playerX * CELL_SIZE;
  const py = playerY * CELL_SIZE;
  ctx.fillStyle = Colors.player;
  ctx.fillText('@', px + 2, py + 1);
}

// Update HUD with current stats
function refreshHUD(): void {
  updateHUD({
    hp: playerHp,
    maxHp: PLAYER_HP,
    air: playerAir,
    maxAir: PLAYER_AIR,
    battery: playerBattery,
    maxBattery: PLAYER_BATTERY,
    coins: playerCoins,
    level: level,
    maxLevel: LAST_LEVEL,
    lightOn: playerLightOn,
  });
}

// Check if a position is walkable
function canMoveTo(y: number, x: number): boolean {
  if (y < 0 || y >= Y_ || x < 0 || x >= X_) {
    return false;
  }
  const type = tileM[y][x].type;
  return type === FLOOR || type === NEXT_LEVEL;
}

// Process player action
function processAction(action: InputAction): boolean {
  switch (action.type) {
    case 'move': {
      const newY = playerY + action.dy;
      const newX = playerX + action.dx;
      if (canMoveTo(newY, newX)) {
        playerY = newY;
        playerX = newX;
        return true;
      }
      return false;
    }
    case 'wait':
      return true;

    case 'toggleLight':
      playerLightOn = !playerLightOn;
      addMessage(`Flashlight ${playerLightOn ? 'ON' : 'OFF'}`, 'info');
      return true;

    case 'useStairs':
      if (tileM[playerY][playerX].type === NEXT_LEVEL) {
        level++;
        if (level > LAST_LEVEL) {
          addMessage('You escaped the crypt! YOU WON!', 'success');
          gameRunning = false;
          return true;
        }
        addMessage(`Descending to level ${level}...`, 'info');
        initMap();
        const start = findStartPosition();
        playerY = start.y;
        playerX = start.x;
        return true;
      }
      addMessage('No stairs here.', 'warning');
      return false;

    case 'help':
      showHelp();
      return false;

    case 'quit':
      addMessage('You quit the game. Game over!', 'danger');
      gameRunning = false;
      return true;

    case 'none':
      return false;
  }
}

// Main game loop
async function gameLoop(): Promise<void> {
  while (gameRunning) {
    // Wait for player input
    const action = await waitForInput();

    // Process action (returns true if turn was consumed)
    const turnTaken = processAction(action);

    if (turnTaken && gameRunning) {
      // Drain battery if light is on
      if (playerLightOn && playerBattery > 0) {
        playerBattery--;
      }

      // Drain air
      playerAir--;
      if (playerAir <= 0) {
        addMessage('You suffocated! Game over!', 'danger');
        gameRunning = false;
      } else if (playerAir <= 21 && playerAir % 5 === 0) {
        addMessage('DANGER - LOW AIR SUPPLY!', 'danger');
      }
    }

    // Redraw
    drawScreen();
    refreshHUD();
  }

  addMessage('Press any key to restart...', 'info');
}

// Main initialization
async function init(): Promise<void> {
  console.log('CryptRover Web initializing...');

  // Initialize RNG
  const seed = initRng();
  console.log(`RNG seed: ${seed}`);

  // Initialize canvas
  initCanvas();

  // Initialize input handling
  initInput();

  // Initialize map (generates dungeon)
  initMap();

  // Find starting position
  const start = findStartPosition();
  playerY = start.y;
  playerX = start.x;

  // Initial draw
  drawScreen();
  refreshHUD();

  // Show welcome message
  addMessage('Welcome to CryptRover! Find the stairs to escape.', 'info');
  addMessage('Press ? for help.', 'info');

  // Show help modal on start
  showHelp();

  console.log('CryptRover Web ready!');
  console.log(`Map size: ${X_}x${Y_}, Cell size: ${CELL_SIZE}px`);

  // Start game loop
  await gameLoop();
}

// Start when DOM is ready
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', () => void init());
} else {
  void init();
}
