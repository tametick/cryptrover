// CryptRover Web - Main entry point

import {
  X_, Y_, CELL_SIZE, Colors, WALL, NEXT_LEVEL, SEEN, IN_SIGHT,
  PLAYER_HP, PLAYER_AIR, PLAYER_BATTERY, LAST_LEVEL, ENTS_, FOV_RADIUS, CORPSE
} from './constants.js';
import { initMap, tileM, tileColorM, viewM } from './map.js';
import { initRng } from './utils.js';
import {
  initInput, waitForInput, showHelp,
  updateHUD, addMessage, type InputAction
} from './io.js';
import {
  initEnts, getPlayer, entL, moveTo, moveAllEnemies,
  isPlayerAlive, fov
} from './entities.js';

// Canvas and context
let canvas: HTMLCanvasElement;
let ctx: CanvasRenderingContext2D;

// Game state
let level = 1;
let turn = 0;
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

// Draw the map grid with FOV
export function drawScreen(): void {
  // Clear canvas
  ctx.fillStyle = Colors.black;
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Draw map tiles based on visibility
  for (let y = 0; y < Y_; y++) {
    for (let x = 0; x < X_; x++) {
      const tile = tileM[y][x];
      const glyph = tile.type;
      const px = x * CELL_SIZE;
      const py = y * CELL_SIZE;
      const visibility = viewM[y][x];

      if (visibility === IN_SIGHT) {
        // Fully visible - use tile color or blood tint
        let color = Colors.floor;
        if (glyph === WALL) {
          color = Colors.wall;
        } else if (glyph === NEXT_LEVEL) {
          color = Colors.stairs;
        } else if (tileColorM[y][x] === Colors.blood) {
          color = Colors.blood;
        }
        ctx.fillStyle = color;
        ctx.fillText(glyph, px + 2, py + 1);
      } else if (visibility === SEEN) {
        // Previously seen - fog of war
        ctx.fillStyle = Colors.fog;
        ctx.fillText(glyph, px + 2, py + 1);
      }
      // UNSEEN tiles are not drawn (black)
    }
  }

  // Draw corpses
  for (let e = 0; e < ENTS_; e++) {
    const ent = entL[e];
    if (!ent.alive && tileM[ent.y][ent.x].type !== NEXT_LEVEL) {
      const px = ent.x * CELL_SIZE;
      const py = ent.y * CELL_SIZE;
      const visibility = viewM[ent.y][ent.x];
      if (visibility === IN_SIGHT) {
        ctx.fillStyle = ent.color;
        ctx.fillText(CORPSE, px + 2, py + 1);
      } else if (visibility === SEEN) {
        ctx.fillStyle = Colors.fog;
        ctx.fillText(CORPSE, px + 2, py + 1);
      }
    }
  }

  // Draw living entities (only if visible)
  for (let e = 0; e < ENTS_; e++) {
    const ent = entL[e];
    if (ent.alive && viewM[ent.y][ent.x] === IN_SIGHT) {
      const px = ent.x * CELL_SIZE;
      const py = ent.y * CELL_SIZE;
      ctx.fillStyle = ent.color;
      ctx.fillText(ent.glyph, px + 2, py + 1);
    }
  }
}

// Decay FOV from IN_SIGHT to SEEN
function decayFov(): void {
  for (let y = 0; y < Y_; y++) {
    for (let x = 0; x < X_; x++) {
      if (viewM[y][x] === IN_SIGHT) {
        viewM[y][x] = SEEN;
      }
    }
  }
}

// Update HUD with current stats
function refreshHUD(): void {
  const player = getPlayer();
  updateHUD({
    hp: player.hp,
    maxHp: PLAYER_HP,
    air: player.air,
    maxAir: PLAYER_AIR,
    battery: player.battery,
    maxBattery: PLAYER_BATTERY,
    coins: player.coins,
    level: level,
    maxLevel: LAST_LEVEL,
    lightOn: player.lightOn,
  });
}

// Process player action
function processAction(action: InputAction): boolean {
  const player = getPlayer();

  switch (action.type) {
    case 'move': {
      return moveTo(player, action.dy, action.dx);
    }
    case 'wait':
      return true;

    case 'toggleLight':
      player.lightOn = !player.lightOn;
      addMessage(`Flashlight ${player.lightOn ? 'ON' : 'OFF'}`, 'info');
      return true;

    case 'useStairs':
      if (tileM[player.y][player.x].type === NEXT_LEVEL) {
        level++;
        if (level > LAST_LEVEL) {
          addMessage('You escaped the crypt! YOU WON!', 'success');
          gameRunning = false;
          return true;
        }
        addMessage(`Descending to level ${level}...`, 'info');
        initMap();
        initEnts(level);
        fov(player.y, player.x, FOV_RADIUS);
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
  const player = getPlayer();

  while (gameRunning) {
    // Wait for player input
    const action = await waitForInput();

    // Process action (returns true if turn was consumed)
    const turnTaken = processAction(action);

    if (turnTaken && gameRunning) {
      turn++;

      // Enemy phase
      moveAllEnemies(turn);

      // Check if player died from enemy attack
      if (!isPlayerAlive()) {
        addMessage('You died! Game over!', 'danger');
        gameRunning = false;
      }

      // Decay FOV
      decayFov();

      // Drain battery if light is on
      if (player.lightOn && player.battery > 0) {
        player.battery--;
      }

      // Drain air
      player.air--;
      if (player.air <= 0) {
        addMessage('You suffocated! Game over!', 'danger');
        gameRunning = false;
      } else if (player.air <= 21 && player.air % 5 === 0) {
        addMessage('DANGER - LOW AIR SUPPLY!', 'danger');
      }

      // Recompute FOV
      fov(player.y, player.x, FOV_RADIUS);
    }

    // Redraw
    drawScreen();
    refreshHUD();
  }

  addMessage('Press F5 to restart...', 'info');
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

  // Initialize entities
  initEnts(level);

  // Initial FOV
  const player = getPlayer();
  fov(player.y, player.x, FOV_RADIUS);

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
