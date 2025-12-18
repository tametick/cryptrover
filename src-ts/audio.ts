// CryptRover Web - Audio stubs
// Empty bodies for now, hookable for future Web Audio API implementation

// Sound effect names matching C codebase
export type SoundName =
  | 'money'    // Coin pickup
  | 'clunk'    // Item pickup (med, air, battery)
  | 'bite'     // Enemy attack
  | 'punch'    // Player attack
  | 'grunt'    // Player hurt/death
  | 'alert';   // Low HP/air warning

// Music track names
export type MusicName = 'theme';

// Play a one-shot sound effect
export function playSound(name: SoundName): void {
  // Stub: implement with Web Audio API later
  console.debug(`[audio] playSound: ${name}`);
}

// Start background music (loops indefinitely)
export function playMusic(name: MusicName): void {
  // Stub: implement with Web Audio API later
  console.debug(`[audio] playMusic: ${name}`);
}

// Stop background music
export function stopMusic(): void {
  // Stub: implement with Web Audio API later
  console.debug('[audio] stopMusic');
}

// Initialize audio system
export function initAudio(): void {
  // Stub: load audio files, set up Web Audio context
  console.debug('[audio] initAudio');
}

// Cleanup audio resources
export function cleanupAudio(): void {
  // Stub: stop all sounds, release resources
  console.debug('[audio] cleanupAudio');
}
