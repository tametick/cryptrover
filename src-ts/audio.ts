// CryptRover Web - Audio system
// Simple implementation using HTMLAudioElement

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

// Preloaded sound effects
const sounds: Map<SoundName, HTMLAudioElement> = new Map();

// Current music element
let music: HTMLAudioElement | null = null;
let audioResumed = false;

// Audio paths
const SOUND_PATH = 'media/';
const MUSIC_FILES: Record<MusicName, string> = {
  theme: 'A_Nightmare_On_Elm_Street.ogg'
};

// Play a one-shot sound effect
export function playSound(name: SoundName): void {
  const audio = sounds.get(name);
  if (audio) {
    // Clone to allow overlapping sounds
    const clone = audio.cloneNode() as HTMLAudioElement;
    clone.volume = 0.5;
    clone.play().catch(() => {/* ignore autoplay errors */});
  }
}

// Start background music (loops indefinitely)
export function playMusic(name: MusicName): void {
  stopMusic();
  const file = MUSIC_FILES[name];
  if (file) {
    music = new Audio(SOUND_PATH + file);
    music.loop = true;
    music.volume = 0.3;
    music.play().catch(() => {/* ignore autoplay errors */});
  }
}

// Stop background music
export function stopMusic(): void {
  if (music) {
    music.pause();
    music.currentTime = 0;
    music = null;
  }
}

// Initialize audio system - preload all sounds
export function initAudio(): void {
  const soundNames: SoundName[] = ['money', 'clunk', 'bite', 'punch', 'grunt', 'alert'];
  for (const name of soundNames) {
    const audio = new Audio(`${SOUND_PATH}${name}.wav`);
    audio.preload = 'auto';
    sounds.set(name, audio);
  }
}

// Cleanup audio resources
export function cleanupAudio(): void {
  stopMusic();
  sounds.clear();
}

// Resume audio after user interaction (handles browser autoplay policy)
export function resumeAudio(): void {
  if (audioResumed) return;
  audioResumed = true;
  if (music) {
    music.play().catch(() => {});
  }
}
