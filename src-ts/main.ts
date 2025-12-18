// CryptRover Web - Main entry point

import {
  X_, Y_, CELL_SIZE, Colors, WALL, NEXT_LEVEL, SEEN, IN_SIGHT,
  PLAYER_HP, PLAYER_AIR, PLAYER_BATTERY, LAST_LEVEL, ENTS_, FOV_RADIUS, CORPSE
} from './constants.js';
import { initMap, tileM, tileColorM, viewM } from './map.js';
import { initRng } from './utils.js';
import {
  initInput, waitForInput, showHelp,
  updateHUD, addMessage, clearMessages, showGameOver,
  type InputAction
} from './io.js';
import {
  initEnts, getPlayer, entL, moveTo, moveAllEnemies,
  isPlayerAlive, fov
} from './entities.js';
import { initItems, itemL, useItem } from './items.js';
import { initAudio, playMusic, playSound } from './audio.js';

// Canvas and context
let canvas: HTMLCanvasElement;
let ctx: CanvasRenderingContext2D;

// Game state
let level = 1;
let turn = 0;
let gameRunning = true;
let playerWon = false;

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

  // Draw items (only if visible)
  for (const item of itemL) {
    if (!item.used) {
      const visibility = viewM[item.y][item.x];
      const px = item.x * CELL_SIZE;
      const py = item.y * CELL_SIZE;
      if (visibility === IN_SIGHT) {
        ctx.fillStyle = item.color;
        ctx.fillText(item.glyph, px + 2, py + 1);
      } else if (visibility === SEEN) {
        ctx.fillStyle = Colors.fog;
        ctx.fillText(item.glyph, px + 2, py + 1);
      }
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
          playerWon = true;
          gameRunning = false;
          return true;
        }
        addMessage(`Descending to level ${level}...`, 'info');
        initMap();
        initEnts(level);
        initItems();
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

// Main game loop - runs continuously, handles restarts internally
async function gameLoop(): Promise<void> {
  // Outer loop for restarts
  while (true) {
    let player = getPlayer();

    // Inner loop for single game session
    while (gameRunning) {
      // Wait for player input
      const action = await waitForInput();

      // Process action (returns true if turn was consumed)
      const turnTaken = processAction(action);

      if (turnTaken && gameRunning) {
        turn++;

        // Use item if player is standing on one
        useItem(player);

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
          playSound('grunt');
          gameRunning = false;
        } else if (player.air <= 21 && player.air % 5 === 0) {
          addMessage('DANGER - LOW AIR SUPPLY!', 'danger');
          playSound('alert');
        }

        // Recompute FOV
        fov(player.y, player.x, FOV_RADIUS);
      }

      // Redraw
      drawScreen();
      refreshHUD();
    }

    // Show game over modal with stats
    await showGameOver(playerWon, {
      gold: player.coins,
      level: level,
      hp: Math.max(0, player.hp),
      maxHp: PLAYER_HP,
      air: Math.max(0, player.air),
      maxAir: PLAYER_AIR,
      battery: Math.max(0, player.battery),
      maxBattery: PLAYER_BATTERY,
    });

    // Reset game state for restart
    resetGameState();
    player = getPlayer();
  }
}

// Reset game state for restart
function resetGameState(): void {
  level = 1;
  turn = 0;
  gameRunning = true;
  playerWon = false;

  // Clear entity list so initEnts creates fresh entities
  entL.length = 0;

  // Clear messages
  clearMessages();

  // Reinitialize everything
  const seed = initRng();
  console.log(`Restarting with seed: ${seed}`);

  initMap();
  initEnts(level);
  initItems();

  const player = getPlayer();
  fov(player.y, player.x, FOV_RADIUS);

  drawScreen();
  refreshHUD();

  addMessage('Welcome to CryptRover! Find the stairs to escape.', 'info');
  addMessage('Press ? for help.', 'info');
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

  // Initialize audio
  initAudio();
  playMusic('theme');

  // Initialize map (generates dungeon)
  initMap();

  // Initialize entities
  initEnts(level);

  // Initialize items
  initItems();

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
