# TypeScript Port Compatibility Fixes

This document lists all changes needed to make the TypeScript version 100% gameplay compatible with the original C version.

## Progress Checklist

- [x] **1. Enemy Movement Timing** - Fix inverted logic (`!== 0` instead of `=== 0`)
- [x] **2. Item Spawn Counts** - Swap BATTERY_NUM and COIN_NUM constants
- [x] **3. Map Generation Loop** - Remove outer loop limit for C parity
- [x] **4. Air Warning Calculation** - Use percentage formula like C version
- [ ] **5. Score Persistence** - Add localStorage-based score saving
- [ ] **6. Player Color** - Change from green to white
- [ ] **7. Blood Splatter Variation** - Add bright/dark red variation
- [ ] **8. HUD Display Format** - Show percentages instead of absolute values
- [ ] **9. Game Over Behavior** - Require explicit restart action

---

## Critical Gameplay Fixes

### 1. Enemy Movement Timing (INVERTED LOGIC)

**File:** `src-ts/entities.ts:295`

**Current (incorrect):**
```typescript
if (enemy.alive && enemy.speed && turn % enemy.speed === 0) {
```

**Fix:**
```typescript
if (enemy.alive && enemy.speed && turn % enemy.speed !== 0) {
```

**Reason:** C version moves enemies when `turn % speed` is truthy (non-zero). With speed=3, enemies should move on turns 1,2,4,5,7,8... not 3,6,9,12...

---

### 2. Item Spawn Counts (Swap Constants)

**File:** `src-ts/constants.ts:45-46`

**Current:**
```typescript
export const BATTERY_NUM = 3;
export const COIN_NUM = 5;
```

**Fix:**
```typescript
export const BATTERY_NUM = 5;
export const COIN_NUM = 3;
```

**Reason:** The C version has a bug that results in 5 batteries and 3 coins per level. Rather than replicate the bug, swap the constants so the actual spawned content matches.

---

### 3. Map Generation Loop Limit (Remove Outer Limit)

**File:** `src-ts/map.ts:127-130`

**Current:**
```typescript
let outerTries = 0;
const MAX_OUTER_TRIES = 100;

while (outerTries++ < MAX_OUTER_TRIES) {
```

**Fix:**
```typescript
while (true) {
```

**Reason:** C version uses infinite loop, relying only on the inner 10000-try limit to exit. The outer limit produces smaller dungeons with fewer rooms.

Also remove the `outerTries` variable and `MAX_OUTER_TRIES` constant if no longer used.

---

### 4. Air Warning Calculation (Use Percentage Formula)

**File:** `src-ts/main.ts:251`

**Current:**
```typescript
} else if (player.air <= 21 && player.air % 5 === 0) {
```

**Fix:**
```typescript
} else if (player.air <= AIR_CHARGE && Math.floor(100 * player.air / PLAYER_AIR) % 5 === 0) {
```

**Reason:** C version calculates percentage of max air and checks if divisible by 5. This fires warnings at different air levels than the raw modulo check.

---

### 5. Score Persistence (Add scores.dat Support)

**File:** `src-ts/main.ts` (new function) and `src-ts/io.ts`

**Add function to save scores:**
```typescript
function saveScore(level: number): void {
  const player = getPlayer();
  const score = {
    gold: player.coins,
    level: level,
    hpPct: Math.floor(100 * player.hp / PLAYER_HP),
    airPct: Math.floor(100 * player.air / PLAYER_AIR),
    batteryPct: Math.floor(100 * player.battery / PLAYER_BATTERY)
  };
  // Store in localStorage or IndexedDB
  const scores = JSON.parse(localStorage.getItem('cryptrover_scores') || '[]');
  scores.push(score);
  localStorage.setItem('cryptrover_scores', JSON.stringify(scores));
}
```

**Call on game end (win or loss).**

**Add high score display in game over modal**, sorted by gold descending, highlighting current score.

---

## Visual/UX Fixes for Full Parity

### 6. Player Color

**File:** `src-ts/constants.ts:71`

**Current:**
```typescript
player: '#00ff00',   // Green
```

**Fix:**
```typescript
player: '#ffffff',   // White (matches C COLOR_WHITE)
```

---

### 7. Blood Splatter Variation

**File:** `src-ts/entities.ts:222-227`

**Current:**
```typescript
if (randInt(0, 3) !== 0) {
  tileColorM[yy][xx] = Colors.blood;
}
```

**Fix:** Add bold/brightness variation like C version:
```typescript
if (randInt(0, 3) !== 0) {
  // C version: random bold (brighter) or normal
  const bright = randInt(0, 2) === 0;
  tileColorM[yy][xx] = bright ? Colors.bloodBright : Colors.blood;
}
```

Also add to constants:
```typescript
bloodBright: '#ff0000',  // Bright red (A_BOLD equivalent)
blood: '#800000',        // Dark red (A_NORMAL)
```

---

### 8. HUD Display Format (Percentages with Bars)

**File:** `src-ts/io.ts:234-291`

**Changes needed:**
- Show percentages instead of absolute values: `"Hit points: 100%"` not `"18/18"`
- Consider ASCII-style bar representation: `[**********]`
- Handle singular/plural for coins: `"1 coin"` vs `"X coins"`

---

### 9. Game Over Behavior

**File:** `src-ts/main.ts:209-281`

**Current:** Shows modal and restarts game in loop.

**Fix for parity:** After showing game over and high scores, the game should end (close tab/window or show static end screen). If restart is desired, require explicit user action (e.g., "Press R to restart").

---


## Summary Checklist

| Priority | Fix | File | Line |
|----------|-----|------|------|
| CRITICAL | Enemy movement `!== 0` | entities.ts | 295 |
| CRITICAL | Swap BATTERY_NUM/COIN_NUM | constants.ts | 45-46 |
| CRITICAL | Remove map outer loop limit | map.ts | 127-130 |
| CRITICAL | Air warning percentage calc | main.ts | 251 |
| HIGH | Add score persistence | main.ts, io.ts | new |
| MEDIUM | Player color white | constants.ts | 71 |
| MEDIUM | Blood splatter variation | entities.ts | 222-227 |
| LOW | HUD percentage display | io.ts | 234-291 |
| LOW | Game over exit behavior | main.ts | 209-281 |
