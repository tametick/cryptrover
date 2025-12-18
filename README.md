# CryptRover

A terminal roguelike where you're an archaeologist trapped in an underground crypt with limited air.

## Introduction

You must survive long enough to reach the crypt's exit.

- Be too rash — and the arachnids will nibble you to death.
- Be too wary — and your air supply will run out.

Luckily, the ancients have left behind med packs, air cans and batteries in the crypt — use them wisely!

## Versions

### Web (TypeScript)

Play in browser. Source in `src-ts/`, compiles to `dist/`.

```bash
npm install
npm run dev      # Watch + serve at http://localhost:8080
npm run build    # Production build
npm run test     # Run tests
```

### Terminal (C99 + ncurses)

Classic terminal version. Source in `src/`.

```bash
./configure && make
./cr
```

Options:
- `--with-pdc` — Use PDCurses instead of ncurses (Linux)
- `--without-sdl` — Disable music/sound effects
- `--debug` — Debug build

PDCurses and SDL dependencies are downloaded automatically by configure when needed.

## Controls

Move or attack:
```
 q w e    7 8 9    y k u
  \|/      \|/      \|/
 a-s-d    4-5-6    h-.-l
  /|\      /|\      /|\
 z x c    1 2 3    b j n
```

| Key | Action |
|-----|--------|
| `<` or `,` | Descend stairs |
| `f` | Toggle flashlight |
| `?` or `F1` | Help |
| `.` or `5` | Wait |
| `ESC` | Quit |

## Documentation

- [game.md](game.md) — Game design
- [code.md](code.md) — C version architecture
- [code-ts.md](code-ts.md) — TypeScript version architecture
