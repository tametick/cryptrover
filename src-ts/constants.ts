// CryptRover Web - Constants (mirroring C defines)

// Map attributes
export const Y_ = 24;
export const X_ = 48;
export const CELL_SIZE = 16; // pixels per cell for canvas rendering

// Tile types
export const WALL = '#';
export const FLOOR = '.';
export const NEXT_LEVEL = '<';

// Map generation parameters
export const ROOM_RADIUS = 2;
export const PATHS = 5;
export const LAST_LEVEL = 12;

// View states
export const UNSEEN = 0;
export const SEEN = 1;
export const IN_SIGHT = 2;

// Entity counts and attributes
export const ENTS_ = 12;
export const FOV_RADIUS = 5;
export const CORPSE = '%';
export const ARACHNID = 'a';
export const PLAYER = '@';

// Item types
export const MED_PACK = '+';
export const AIR_CAN = '*';
export const BATTERY = '!';
export const COIN = '$';

// Item charge values
export const MED_CHARGE = 3;
export const AIR_CHARGE = 21;
export const BATTERY_CHARGE = 36;
export const COIN_CHARGE = 1;

// Item counts per level
export const MED_NUM = 3;
export const AIR_NUM = 4;
export const BATTERY_NUM = 3;
export const COIN_NUM = 5;
export const ITEMS_ = MED_NUM + AIR_NUM + BATTERY_NUM + COIN_NUM;

// Player stats
export const PLAYER_HP = MED_CHARGE * 6;       // 18
export const PLAYER_AIR = AIR_CHARGE * 5;      // 105
export const PLAYER_BATTERY = BATTERY_CHARGE * 5; // 180

// Key codes
export const ESC = 27;
export const CTRL_C = 3;

// Colors (CSS color strings)
export const Colors: Record<string, string> = {
  black: '#000000',
  white: '#ffffff',
  red: '#ff0000',
  green: '#00ff00',
  blue: '#0080ff',
  yellow: '#ffff00',
  magenta: '#ff00ff',
  cyan: '#00ffff',
  gray: '#808080',
  darkGray: '#404040',
  fog: '#404040',
  seen: '#606060',
  floor: '#808080',
  wall: '#a0a0a0',
  player: '#00ff00',
  enemy: '#ff0000',
  blood: '#800000',
  med: '#00ff00',
  air: '#0080ff',
  battery: '#ff00ff',
  coin: '#ffff00',
  stairs: '#00ffff',
};
