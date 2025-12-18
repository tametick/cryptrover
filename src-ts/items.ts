// CryptRover Web - Item management

import {
  Y_, X_, WALL, NEXT_LEVEL, ITEMS_,
  MED_PACK, AIR_CAN, BATTERY, COIN,
  MED_NUM, AIR_NUM, BATTERY_NUM,
  MED_CHARGE, AIR_CHARGE, BATTERY_CHARGE, COIN_CHARGE,
  PLAYER_HP, PLAYER_AIR, PLAYER_BATTERY,
  Colors
} from './constants.js';
import { type Item, type Ent, makeGrid } from './types.js';
import { tileM } from './map.js';
import { entM } from './entities.js';
import { randInt, min } from './utils.js';
import { addMessage } from './io.js';
import { playSound } from './audio.js';

// Global item state
export let itemL: Item[] = [];
export let itemM: (Item | null)[][] = [];

// Initialize items for a level
export function initItems(): void {
  // Clear item placement map
  itemM = makeGrid(Y_, X_, () => null);

  // Create item list
  itemL = [];

  for (let i = 0; i < ITEMS_; i++) {
    // Find valid position (not on wall, stairs, entity, or other item)
    let y: number;
    let x: number;
    do {
      y = randInt(0, Y_ - 1);
      x = randInt(0, X_ - 1);
    } while (
      tileM[y][x].type === WALL ||
      tileM[y][x].type === NEXT_LEVEL ||
      entM[y][x] !== null ||
      itemM[y][x] !== null
    );

    // Determine item type based on index
    let glyph: string;
    let type: string;
    let color: string;

    if (i < MED_NUM) {
      glyph = MED_PACK;
      type = 'med';
      color = Colors.med;
    } else if (i < MED_NUM + AIR_NUM) {
      glyph = AIR_CAN;
      type = 'air';
      color = Colors.air;
    } else if (i < MED_NUM + AIR_NUM + BATTERY_NUM) {
      glyph = BATTERY;
      type = 'battery';
      color = Colors.battery;
    } else {
      glyph = COIN;
      type = 'coin';
      color = Colors.coin;
    }

    const item: Item = {
      x,
      y,
      glyph,
      type,
      color,
      used: false,
    };

    itemL.push(item);
    itemM[y][x] = item;
  }
}

// Use item at player's position (auto-consume)
export function useItem(player: Ent): void {
  const item = itemM[player.y][player.x];

  if (item === null || item.used) {
    return;
  }

  switch (item.type) {
    case 'med':
      if (player.hp < PLAYER_HP) {
        player.hp = min(player.hp + MED_CHARGE, PLAYER_HP);
        item.used = true;
        itemM[player.y][player.x] = null;
        addMessage('You feel healthy.', 'success');
        playSound('clunk');
      } else {
        addMessage('A med pack.', '');
      }
      break;

    case 'air':
      if (player.air < PLAYER_AIR) {
        player.air = min(player.air + AIR_CHARGE, PLAYER_AIR);
        item.used = true;
        itemM[player.y][player.x] = null;
        addMessage('You replenish your air supply.', 'info');
        playSound('clunk');
      } else {
        addMessage('An air canister.', '');
      }
      break;

    case 'battery':
      if (player.battery < PLAYER_BATTERY) {
        player.battery = min(player.battery + BATTERY_CHARGE, PLAYER_BATTERY);
        item.used = true;
        itemM[player.y][player.x] = null;
        addMessage('You charge your battery.', 'info');
        playSound('clunk');
      } else {
        addMessage('A battery.', '');
      }
      break;

    case 'coin':
      player.coins += COIN_CHARGE;
      item.used = true;
      itemM[player.y][player.x] = null;
      addMessage("You've found a gold coin.", 'success');
      playSound('money');
      break;
  }
}
