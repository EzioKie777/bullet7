# AGENTS

This repository is a small C++/SFML game named Bullet7. There is no existing README or project documentation, so this file provides the key facts an AI coding agent needs to be productive.

## Build and Run
- Uses native C++ compilation with SFML libraries.
- Recommended build command:
  ```bash
g++ -g <source-file>.cpp -o <output> -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
  ```
- VS Code build task: `Build C++` in `.vscode/tasks.json`
- The code expects an `assets/` directory at the repository root with textures, fonts, and audio.

## Project structure
- `main.cpp` creates and runs `Game`.
- `Game.hpp` / `Game.cpp` contain the main loop, event handling, rendering, and game state flow.
- `Player.hpp` / `Player.cpp`, `World.hpp` / `World.cpp`, `HUD.hpp`, and `UI.hpp` implement game objects and UI components.
- `Assets.hpp` is a central asset manager that loads textures, fonts, and sound buffers on demand.
- `Settings.hpp` defines global player controls and game settings via `gSettings`.

## Important conventions
- The code uses `sf::RenderWindow`, `sf::Music`, `sf::Texture`, `sf::Font`, and other SFML classes directly.
- Asset lookup uses names like `"p1_idle"` and resolves them to `assets/<name>.png` or `.wav`.
- Input rebinding is done in `Game.cpp` through `PlayerSettings` and `sf::Event::KeyPressed`.
- Game flow is state-driven via `AppState` and per-state `update*` / `render*` methods.

## Notes for AI agents
- Do not assume there is a full asset set in the workspace; runtime assets may be missing.
- Prefer small, incremental changes with attention to SFML resource management and event loops.
- There is no higher-level documentation in the repo, so use the source files as the authoritative reference.
