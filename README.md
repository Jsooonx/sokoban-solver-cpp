# Sokoban Game

Implementasi C++17 + SFML dari `SOKOBAN_SPEC.md`.

## Build

```bash
cmake -S . -B build
cmake --build build
./build/sokoban
```

## Controls

- Arrow keys: move player
- `Z`: undo last move, max 50
- `R`: restart level
- `Space`: use hint
- `H`: toggle hint display
- `P`: pause
- `Esc`: back to main menu
- Menu: `Up/Down`, `Enter`, `L`, `S`, `T`, `A`

Scores are stored locally in `data/leaderboard.json`.
