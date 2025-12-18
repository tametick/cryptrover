// CryptRover Web - Input/Output handling

// Input action types
export type InputAction =
  | { type: 'move'; dy: number; dx: number }
  | { type: 'wait' }
  | { type: 'toggleLight' }
  | { type: 'useStairs' }
  | { type: 'help' }
  | { type: 'quit' }
  | { type: 'none' };

// Key to action mapping
const keyMap: Record<string, InputAction> = {
  // Movement - vi keys (hjklyubn)
  KeyH: { type: 'move', dy: 0, dx: -1 },  // left
  KeyJ: { type: 'move', dy: 1, dx: 0 },   // down
  KeyK: { type: 'move', dy: -1, dx: 0 },  // up
  KeyL: { type: 'move', dy: 0, dx: 1 },   // right
  KeyY: { type: 'move', dy: -1, dx: -1 }, // up-left
  KeyU: { type: 'move', dy: -1, dx: 1 },  // up-right
  KeyB: { type: 'move', dy: 1, dx: -1 },  // down-left
  KeyN: { type: 'move', dy: 1, dx: 1 },   // down-right

  // Movement - WASD + QE/ZC
  KeyW: { type: 'move', dy: -1, dx: 0 },  // up
  KeyA: { type: 'move', dy: 0, dx: -1 },  // left
  KeyS: { type: 'wait' },                  // wait (s in center)
  KeyD: { type: 'move', dy: 0, dx: 1 },   // right
  KeyX: { type: 'move', dy: 1, dx: 0 },   // down
  KeyQ: { type: 'move', dy: -1, dx: -1 }, // up-left
  KeyE: { type: 'move', dy: -1, dx: 1 },  // up-right
  KeyZ: { type: 'move', dy: 1, dx: -1 },  // down-left
  KeyC: { type: 'move', dy: 1, dx: 1 },   // down-right

  // Movement - Arrow keys
  ArrowUp: { type: 'move', dy: -1, dx: 0 },
  ArrowDown: { type: 'move', dy: 1, dx: 0 },
  ArrowLeft: { type: 'move', dy: 0, dx: -1 },
  ArrowRight: { type: 'move', dy: 0, dx: 1 },

  // Movement - Numpad
  Numpad8: { type: 'move', dy: -1, dx: 0 },  // up
  Numpad2: { type: 'move', dy: 1, dx: 0 },   // down
  Numpad4: { type: 'move', dy: 0, dx: -1 },  // left
  Numpad6: { type: 'move', dy: 0, dx: 1 },   // right
  Numpad7: { type: 'move', dy: -1, dx: -1 }, // up-left
  Numpad9: { type: 'move', dy: -1, dx: 1 },  // up-right
  Numpad1: { type: 'move', dy: 1, dx: -1 },  // down-left
  Numpad3: { type: 'move', dy: 1, dx: 1 },   // down-right
  Numpad5: { type: 'wait' },                  // wait

  // Wait
  Period: { type: 'wait' },
  Digit5: { type: 'wait' },

  // Toggle flashlight
  KeyF: { type: 'toggleLight' },

  // Use stairs
  Comma: { type: 'useStairs' },

  // Help
  Slash: { type: 'help' }, // ? key (shift+/)
  F1: { type: 'help' },

  // Quit
  Escape: { type: 'quit' },
};

// Handle shift+/ for ? (help)
function getKeyCode(e: KeyboardEvent): string {
  // Handle ? key (shift + /)
  if (e.key === '?') {
    return 'Slash';
  }
  // Handle < key (shift + ,) for stairs
  if (e.key === '<') {
    return 'Comma';
  }
  return e.code;
}

// Current pending resolve for input promise
let inputResolve: ((action: InputAction) => void) | null = null;
let helpModalVisible = false;

// Process keyboard event
function handleKeyDown(e: KeyboardEvent): void {
  // Ignore if help modal is visible (any key closes it)
  if (helpModalVisible) {
    hideHelp();
    e.preventDefault();
    return;
  }

  // Game over modal: any key triggers restart
  if (gameOverModalVisible) {
    hideGameOver();
    e.preventDefault();
    return;
  }

  const code = getKeyCode(e);
  const action = keyMap[code];

  if (action && inputResolve) {
    e.preventDefault();
    const resolve = inputResolve;
    inputResolve = null;
    resolve(action);
  }
}

// Wait for next valid input
export function waitForInput(): Promise<InputAction> {
  return new Promise((resolve) => {
    inputResolve = resolve;
  });
}

// Initialize input handling
export function initInput(): void {
  document.addEventListener('keydown', handleKeyDown);
}

// Help modal functions
export function showHelp(): void {
  const modal = document.getElementById('help-modal');
  if (modal) {
    modal.classList.remove('hidden');
    helpModalVisible = true;
  }
}

export function hideHelp(): void {
  const modal = document.getElementById('help-modal');
  if (modal) {
    modal.classList.add('hidden');
    helpModalVisible = false;
  }
}

export function isHelpVisible(): boolean {
  return helpModalVisible;
}

// Game Over modal functions
let gameOverModalVisible = false;
let gameOverResolve: (() => void) | null = null;

export function showGameOver(won: boolean, stats: {
  gold: number;
  level: number;
  hp: number;
  maxHp: number;
  air: number;
  maxAir: number;
  battery: number;
  maxBattery: number;
}): Promise<void> {
  return new Promise((resolve) => {
    const modal = document.getElementById('gameover-modal');
    const title = document.getElementById('gameover-title');
    const goldEl = document.getElementById('go-gold');
    const levelEl = document.getElementById('go-level');
    const hpEl = document.getElementById('go-hp');
    const airEl = document.getElementById('go-air');
    const batteryEl = document.getElementById('go-battery');

    if (modal && title) {
      title.textContent = won ? 'YOU HAVE WON! :)' : 'YOU HAVE LOST! :(';
      title.className = won ? 'win' : 'lose';

      if (goldEl) goldEl.textContent = String(stats.gold);
      if (levelEl) levelEl.textContent = String(stats.level);
      if (hpEl) hpEl.textContent = `${Math.floor(100 * stats.hp / stats.maxHp)}%`;
      if (airEl) airEl.textContent = `${Math.floor(100 * stats.air / stats.maxAir)}%`;
      if (batteryEl) batteryEl.textContent = `${Math.floor(100 * stats.battery / stats.maxBattery)}%`;

      modal.classList.remove('hidden');
      gameOverModalVisible = true;
      gameOverResolve = resolve;
    }
  });
}

export function hideGameOver(): void {
  const modal = document.getElementById('gameover-modal');
  if (modal) {
    modal.classList.add('hidden');
    gameOverModalVisible = false;
    if (gameOverResolve) {
      gameOverResolve();
      gameOverResolve = null;
    }
  }
}

export function isGameOverVisible(): boolean {
  return gameOverModalVisible;
}

// Message log
const messages: Array<{ text: string; cssClass: string }> = [];
const MAX_MESSAGES = 50;

export function addMessage(text: string, cssClass: string = ''): void {
  messages.push({ text, cssClass });
  if (messages.length > MAX_MESSAGES) {
    messages.shift();
  }
  renderMessages();
}

export function clearMessages(): void {
  messages.length = 0;
  renderMessages();
}

function renderMessages(): void {
  const container = document.getElementById('messages');
  if (!container) return;

  container.innerHTML = messages
    .map((m) => `<div class="message ${m.cssClass}">${m.text}</div>`)
    .join('');

  // Scroll to bottom
  container.scrollTop = container.scrollHeight;
}

// HUD update functions
export function updateHUD(stats: {
  hp: number;
  maxHp: number;
  air: number;
  maxAir: number;
  battery: number;
  maxBattery: number;
  coins: number;
  level: number;
  maxLevel: number;
  lightOn: boolean;
}): void {
  // HP bar
  const hpBar = document.getElementById('hp-bar');
  const hpText = document.getElementById('hp-text');
  if (hpBar && hpText) {
    const hpPercent = (stats.hp / stats.maxHp) * 100;
    hpBar.style.width = `${hpPercent}%`;
    hpText.textContent = `${stats.hp}/${stats.maxHp}`;
  }

  // Air bar
  const airBar = document.getElementById('air-bar');
  const airText = document.getElementById('air-text');
  if (airBar && airText) {
    const airPercent = (stats.air / stats.maxAir) * 100;
    airBar.style.width = `${airPercent}%`;
    airText.textContent = `${stats.air}/${stats.maxAir}`;
  }

  // Battery bar
  const batteryBar = document.getElementById('battery-bar');
  const batteryText = document.getElementById('battery-text');
  if (batteryBar && batteryText) {
    const batteryPercent = (stats.battery / stats.maxBattery) * 100;
    batteryBar.style.width = `${batteryPercent}%`;
    batteryText.textContent = `${stats.battery}/${stats.maxBattery}`;
  }

  // Coins
  const coinsText = document.getElementById('coins-text');
  if (coinsText) {
    coinsText.textContent = String(stats.coins);
  }

  // Level
  const levelText = document.getElementById('level-text');
  if (levelText) {
    levelText.textContent = `${stats.level}/${stats.maxLevel}`;
  }

  // Light status
  const lightText = document.getElementById('light-text');
  if (lightText) {
    lightText.textContent = stats.lightOn ? 'ON' : 'OFF';
    lightText.style.color = stats.lightOn ? '#ffff00' : '#808080';
  }
}
