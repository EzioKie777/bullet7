#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "AppState.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "HUD.hpp"
#include "UI.hpp"
#include "Settings.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void updateMainMenu   (float dt);
    void updateMapSelect  (float dt);
    void updateOptions    (float dt);
    void updateCountdown  (float dt);
    void updatePlaying    (float dt);
    void updateScoreboard (float dt);
    void updateWinnerScreen(float dt);

    void renderMainMenu();
    void renderMapSelect();
    void renderOptions();
    void renderCountdown();
    void renderPlaying();
    void renderScoreboard();
    void renderWinnerScreen();

    void startRound();
    void endRound(int winnerIndex);
    void applyScreenShake(float dt);
    void applyWindowMode();

    sf::RenderWindow window;
    World  world;
    Player p1, p2;
    HUD    hud;

    AppState state = AppState::MAIN_MENU;

    int   p1Wins      = 0;
    int   p2Wins      = 0;
    int   roundWinner = 0;

    float countdownTimer = 0.f;
    int   countdownStep  = 3;

    float        shakeTimer  = 0.f;
    sf::Vector2f shakeOffset;

    bool p1Ready     = false;
    bool p2Ready     = false;
    bool p1ReadyPrev = false;
    bool p2ReadyPrev = false;

    // Options screen state
    int  rebindingPlayer = 0;
    int  rebindingAction = -1;
    bool waitingForKey   = false;
    bool editingP1Name   = false;
    bool editingP2Name   = false;

    sf::Music menuMusic;
    sf::Music fightMusic;
    bool menuMusicLoaded  = false;
    bool fightMusicLoaded = false;

    sf::Event currentEvent;
    bool      hasEvent = false;
};
