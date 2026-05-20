# AGENTS

This repository is a small C++/SFML game named Bullet7. There is no README or project documentation, so this file is the primary guide for AI coding agents.

## Build and Run
- Builds with native `g++` and SFML libraries.
- Recommended command:
  ```bash
g++ -g main.cpp Game.cpp Player.cpp World.cpp Bullet.cpp HUD.cpp -o bullet7 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
  ```
- VS Code build task: `Build Bullet7` in `.vscode/tasks.json`.
- No CMake, package manager, or external build system is present.
- Runtime assets are expected in `assets/` at the repository root; missing assets may cause load failures.

## Key files
- `main.cpp` starts the game by constructing `Game` and calling `run()`.
- `Game.hpp` / `Game.cpp` contain the main loop, event processing, game state updates, render dispatch, and menu/game flow.
- `Player.hpp` / `Player.cpp` implement player behavior and controls.
- `World.hpp` / `World.cpp` implement map logic, round state, and physics-related world behavior.
- `HUD.hpp` / `HUD.cpp` and `UI.hpp` render user interface, buttons, and overlays.
- `Assets.hpp` is the central asset manager. `Assets::get().loadAll()` loads textures, fonts, and sound buffers.
- `Settings.hpp` defines global `gSettings` and player keybindings.
- `AppState.hpp` defines the state machine: `MAIN_MENU`, `MAP_SELECT`, `OPTIONS`, `COUNTDOWN`, `PLAYING`, `SCOREBOARD`, `WINNER_SCREEN`, `QUIT`.

## Conventions
- The code uses SFML types directly: `sf::RenderWindow`, `sf::Texture`, `sf::Font`, `sf::Music`, `sf::Event`, and `sf::Keyboard::Key`.
- The game is state-driven; update/render behavior is selected by `AppState` in `Game::update()` and `Game::render()`.
- Input and key rebinding are handled in `Game::processEvents()`.
- Visual effects like screen shake and countdown transitions are implemented inside `Game`.
- Asset keys are resolved through `Assets::get().texture("...")`, `font("...")`, and `soundBuffer("...")`.

## Notes for AI agents
- Do not invent or assume a complete asset set; code fixes should focus on logic and structure unless asset file handling is explicit.
- Prefer small, incremental changes and preserve the existing SFML event/render semantics.
- Use the source files as the authoritative reference for game flow and feature behavior.
- Keep the UI and state machine consistent when adding or changing gameplay features.
