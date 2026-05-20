#include "Game.hpp"
#include "Assets.hpp"
#include "Constants.hpp"
#include <cmath>
#include <cstdlib>
#include <string>

Game::Game()
    : window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Bullet7"),
      p1(sf::Vector2f(300.f, 500.f), 1),
      p2(sf::Vector2f(980.f, 500.f), 2)
{
    window.setFramerateLimit(60);
    Assets::get().loadAll();

    menuMusicLoaded  = menuMusic.openFromFile("assets/menu_music.wav");
    fightMusicLoaded = fightMusic.openFromFile("assets/fight_music.wav");

    if (menuMusicLoaded) {
        menuMusic.setLoop(true);
        menuMusic.setVolume(gSettings.musicVolume);
        menuMusic.play();
    }
}

void Game::applyWindowMode() {
    if (gSettings.fullscreen)
        window.create(sf::VideoMode::getFullscreenModes()[0], "Bullet7", sf::Style::Fullscreen);
    else
        window.create(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Bullet7", sf::Style::Default);
    window.setFramerateLimit(60);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen() && state != AppState::QUIT) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    hasEvent = false;
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) state = AppState::QUIT;
        currentEvent = e;
        hasEvent = true;

        // Name text input
        if (e.type == sf::Event::TextEntered && (editingP1Name || editingP2Name)) {
            std::string& name = editingP1Name ? gSettings.p1.name : gSettings.p2.name;
            if (e.text.unicode == 8) {
                if (!name.empty()) name.pop_back();
            } else if (e.text.unicode >= 32 && e.text.unicode < 128
                       && (int)name.size() < MAX_NAME_LEN) {
                name += (char)e.text.unicode;
            }
        }

        // Keybinding capture
        if (waitingForKey && e.type == sf::Event::KeyPressed) {
            PlayerSettings& keys = (rebindingPlayer == 1) ? gSettings.p1 : gSettings.p2;
            sf::Keyboard::Key* ptrs[] = {
                &keys.keyLeft, &keys.keyRight, &keys.keyJump,
                &keys.keyRun,  &keys.keyFire
            };
            *ptrs[rebindingAction] = e.key.code;
            waitingForKey   = false;
            rebindingAction = -1;
        }

        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Return)
            editingP1Name = editingP2Name = false;
    }
}

void Game::update(float dt) {
    applyScreenShake(dt);
    switch (state) {
        case AppState::MAIN_MENU:     updateMainMenu(dt);     break;
        case AppState::MAP_SELECT:    updateMapSelect(dt);    break;
        case AppState::OPTIONS:       updateOptions(dt);      break;
        case AppState::COUNTDOWN:     updateCountdown(dt);    break;
        case AppState::PLAYING:       updatePlaying(dt);      break;
        case AppState::SCOREBOARD:    updateScoreboard(dt);   break;
        case AppState::WINNER_SCREEN: updateWinnerScreen(dt); break;
        default: break;
    }
}

void Game::render() {
    sf::View view = window.getDefaultView();
    view.setCenter(WIN_WIDTH / 2.f + shakeOffset.x,
                   WIN_HEIGHT / 2.f + shakeOffset.y);
    window.setView(view);
    window.clear(sf::Color(25, 20, 30));

    switch (state) {
        case AppState::MAIN_MENU:     renderMainMenu();    break;
        case AppState::MAP_SELECT:    renderMapSelect();   break;
        case AppState::OPTIONS:       renderOptions();     break;
        case AppState::COUNTDOWN:     renderPlaying(); renderCountdown(); break;
        case AppState::PLAYING:       renderPlaying();     break;
        case AppState::SCOREBOARD:    renderPlaying(); renderScoreboard(); break;
        case AppState::WINNER_SCREEN: renderWinnerScreen(); break;
        default: break;
    }

    window.display();
}

void Game::applyScreenShake(float dt) {
    if (shakeTimer > 0.f) {
        shakeTimer -= dt;
        float mag = SCREENSHAKE_MAG * (shakeTimer / SCREENSHAKE_TIME);
        shakeOffset = sf::Vector2f(
            (float)(std::rand() % 100 - 50) / 50.f * mag,
            (float)(std::rand() % 100 - 50) / 50.f * mag);
    } else {
        shakeOffset = sf::Vector2f(0.f, 0.f);
    }
}

void Game::startRound() {
    p1.reset(sf::Vector2f(300.f, 500.f));
    p2.reset(sf::Vector2f(980.f, 500.f));
    countdownStep  = 3;
    countdownTimer = COUNTDOWN_DELAY;
    state = AppState::COUNTDOWN;

    if (fightMusicLoaded) {
        menuMusic.stop();
        fightMusic.setLoop(true);
        fightMusic.setVolume(gSettings.musicVolume);
        fightMusic.play();
    }
}

void Game::endRound(int winner) {
    roundWinner = winner;
    if (winner == 1) p1Wins++;
    else             p2Wins++;

    shakeTimer = SCREENSHAKE_TIME;

    if (p1Wins >= ROUNDS_TO_WIN || p2Wins >= ROUNDS_TO_WIN) {
        state = AppState::WINNER_SCREEN;
        if (fightMusicLoaded) fightMusic.stop();
        if (menuMusicLoaded) {
            menuMusic.setVolume(gSettings.musicVolume);
            menuMusic.play();
        }
    } else {
        p1Ready = p2Ready = p1ReadyPrev = p2ReadyPrev = false;
        state = AppState::SCOREBOARD;
    }
}

// ── MAIN MENU ─────────────────────────────────────────────────────────────────

void Game::updateMainMenu(float dt) {}

void Game::renderMainMenu() {
    sf::Texture* bg = Assets::get().texture("menu_bg");
    if (bg) {
        sf::Sprite s(*bg);
        s.setScale((float)WIN_WIDTH  / bg->getSize().x,
                   (float)WIN_HEIGHT / bg->getSize().y);
        window.draw(s);
    }

    sf::Texture* logo = Assets::get().texture("logo");
    if (logo) {
        sf::Sprite s(*logo);
        sf::FloatRect lb = s.getLocalBounds();
        s.setOrigin(lb.width / 2.f, lb.height / 2.f);
        s.setPosition((float)WIN_WIDTH / 2.f, 160.f);
        window.draw(s);
    } else {
        drawCenteredText(window, "BULLET7", 140.f, 80, sf::Color(255, 220, 50));
    }

    float cx = (float)WIN_WIDTH / 2.f;
    Button btnStart("START GAME", cx, 320.f);
    Button btnMap  ("MAP SELECT", cx, 395.f);
    Button btnOpts ("OPTIONS",    cx, 470.f);
    Button btnExit ("EXIT",       cx, 545.f);

    btnStart.update(window); btnMap.update(window);
    btnOpts.update(window);  btnExit.update(window);

    if (hasEvent) {
        if (btnStart.isClicked(currentEvent, window)) { p1Wins = p2Wins = 0; startRound(); }
        if (btnMap.isClicked  (currentEvent, window))   state = AppState::MAP_SELECT;
        if (btnOpts.isClicked (currentEvent, window))   state = AppState::OPTIONS;
        if (btnExit.isClicked (currentEvent, window))   state = AppState::QUIT;
    }

    btnStart.draw(window); btnMap.draw(window);
    btnOpts.draw(window);  btnExit.draw(window);
}

// ── MAP SELECT ────────────────────────────────────────────────────────────────

void Game::updateMapSelect(float dt) {}

void Game::renderMapSelect() {
    drawCenteredText(window, "MAP SELECT", 120.f, 56);
    drawCenteredText(window, "Arena 1",    300.f, 36, sf::Color(200, 200, 200));
    drawCenteredText(window, "(More maps coming soon)", 360.f, 22, sf::Color(120, 120, 120));

    float cx = (float)WIN_WIDTH / 2.f;
    Button btnPlay("PLAY THIS MAP", cx, 460.f);
    Button btnBack("BACK",          cx, 540.f);

    btnPlay.update(window); btnBack.update(window);
    if (hasEvent) {
        if (btnPlay.isClicked(currentEvent, window)) { p1Wins = p2Wins = 0; startRound(); }
        if (btnBack.isClicked(currentEvent, window))   state = AppState::MAIN_MENU;
    }
    btnPlay.draw(window); btnBack.draw(window);
}

// ── OPTIONS ───────────────────────────────────────────────────────────────────

void Game::updateOptions(float dt) {}

void Game::renderOptions() {
    drawCenteredText(window, "OPTIONS", 50.f, 48);

    sf::Font* f = Assets::get().font();
    if (!f) {
        Button btnBack("BACK", (float)WIN_WIDTH / 2.f, 600.f);
        btnBack.update(window);
        if (hasEvent && btnBack.isClicked(currentEvent, window))
            state = AppState::MAIN_MENU;
        btnBack.draw(window);
        return;
    }

    float leftCol = 150.f;
    float y = 120.f;

    // ── Name fields ───────────────────────────────────────────────────────────
    auto drawNameField = [&](const std::string& label, std::string& name,
                              bool& editing, float fx) {
        sf::Text lbl; lbl.setFont(*f); lbl.setString(label);
        lbl.setCharacterSize(18); lbl.setFillColor(sf::Color(180, 180, 180));
        lbl.setPosition(fx, y - 22.f); window.draw(lbl);

        sf::RectangleShape box(sf::Vector2f(160.f, 34.f));
        box.setPosition(fx, y);
        box.setFillColor(editing ? sf::Color(60, 60, 90) : sf::Color(40, 40, 60));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(editing ? sf::Color(120, 120, 255) : sf::Color(80, 80, 100));
        window.draw(box);

        sf::Text nt; nt.setFont(*f);
        nt.setString(editing ? name + "|" : name);
        nt.setCharacterSize(20); nt.setFillColor(sf::Color::White);
        nt.setPosition(fx + 8.f, y + 6.f); window.draw(nt);

        if (hasEvent && currentEvent.type == sf::Event::MouseButtonReleased) {
            sf::Vector2f m = window.mapPixelToCoords(
                sf::Vector2i(currentEvent.mouseButton.x, currentEvent.mouseButton.y));
            editing = box.getGlobalBounds().contains(m);
        }
    };

    drawNameField("Player 1 Name", gSettings.p1.name, editingP1Name, leftCol);
    drawNameField("Player 2 Name", gSettings.p2.name, editingP2Name, (float)WIN_WIDTH - 310.f);
    y += 70.f;

    // ── Volume ────────────────────────────────────────────────────────────────
    auto drawVolRow = [&](const std::string& label, float& vol, float ry) {
        sf::Text lbl; lbl.setFont(*f); lbl.setString(label);
        lbl.setCharacterSize(20); lbl.setFillColor(sf::Color::White);
        lbl.setPosition(leftCol, ry); window.draw(lbl);

        Button minus("-", (float)WIN_WIDTH / 2.f - 80.f, ry + 12.f, 40.f, 30.f);
        Button plus ("+", (float)WIN_WIDTH / 2.f + 80.f, ry + 12.f, 40.f, 30.f);
        minus.update(window); plus.update(window);
        if (hasEvent) {
            if (minus.isClicked(currentEvent, window)) vol = std::max(0.f,   vol - 10.f);
            if (plus.isClicked (currentEvent, window)) vol = std::min(100.f, vol + 10.f);
        }
        minus.draw(window); plus.draw(window);

        sf::Text val; val.setFont(*f);
        val.setString(std::to_string((int)vol));
        val.setCharacterSize(20); val.setFillColor(sf::Color(220, 220, 100));
        val.setPosition((float)WIN_WIDTH / 2.f - 10.f, ry); window.draw(val);
    };

    drawVolRow("Music Volume", gSettings.musicVolume, y); y += 50.f;
    drawVolRow("SFX Volume",   gSettings.sfxVolume,   y); y += 60.f;

    // ── Fullscreen ────────────────────────────────────────────────────────────
    {
        sf::Text lbl; lbl.setFont(*f);
        lbl.setString("Fullscreen: " + std::string(gSettings.fullscreen ? "ON" : "OFF"));
        lbl.setCharacterSize(20); lbl.setFillColor(sf::Color::White);
        lbl.setPosition(leftCol, y); window.draw(lbl);
        Button tog("TOGGLE", (float)WIN_WIDTH / 2.f + 80.f, y + 12.f, 140.f, 30.f);
        tog.update(window);
        if (hasEvent && tog.isClicked(currentEvent, window)) {
            gSettings.fullscreen = !gSettings.fullscreen;
            applyWindowMode();
        }
        tog.draw(window);
        y += 50.f;
    }

    // ── Keybindings ───────────────────────────────────────────────────────────
    static const char* actionNames[] = { "Left", "Right", "Jump", "Run", "Fire" };

    auto drawBindRow = [&](PlayerSettings& keys, int pIdx, float startX, float rowY) {
        sf::Keyboard::Key* ptrs[] = {
            &keys.keyLeft, &keys.keyRight, &keys.keyJump, &keys.keyRun, &keys.keyFire
        };
        for (int i = 0; i < 5; i++) {
            float bx = startX + i * 110.f;
            bool waiting = waitingForKey && rebindingPlayer == pIdx && rebindingAction == i;

            sf::Text act; act.setFont(*f); act.setString(actionNames[i]);
            act.setCharacterSize(13); act.setFillColor(sf::Color(160, 160, 160));
            act.setPosition(bx, rowY - 18.f); window.draw(act);

            std::string keyName = waiting ? "..." :
                std::string(1, (char)('A' + (int)*ptrs[i]));
            Button btn(keyName, bx + 45.f, rowY + 14.f, 90.f, 28.f);
            btn.update(window);
            if (hasEvent && btn.isClicked(currentEvent, window) && !waitingForKey) {
                rebindingPlayer = pIdx;
                rebindingAction = i;
                waitingForKey   = true;
            }
            btn.draw(window);
        }
    };

    sf::Text p1lbl; p1lbl.setFont(*f); p1lbl.setString("P1 Keys:");
    p1lbl.setCharacterSize(18); p1lbl.setFillColor(sf::Color(150, 190, 255));
    p1lbl.setPosition(leftCol, y); window.draw(p1lbl);
    drawBindRow(gSettings.p1, 1, leftCol, y + 20.f);
    y += 60.f;

    sf::Text p2lbl; p2lbl.setFont(*f); p2lbl.setString("P2 Keys:");
    p2lbl.setCharacterSize(18); p2lbl.setFillColor(sf::Color(255, 150, 150));
    p2lbl.setPosition(leftCol, y); window.draw(p2lbl);
    drawBindRow(gSettings.p2, 2, leftCol, y + 20.f);
    y += 70.f;

    Button btnBack("BACK", (float)WIN_WIDTH / 2.f, y);
    btnBack.update(window);
    if (hasEvent && btnBack.isClicked(currentEvent, window)) {
        editingP1Name = editingP2Name = false;
        state = AppState::MAIN_MENU;
    }
    btnBack.draw(window);
}

// ── COUNTDOWN ─────────────────────────────────────────────────────────────────

void Game::updateCountdown(float dt) {
    countdownTimer -= dt;
    if (countdownTimer <= 0.f) {
        countdownStep--;
        countdownTimer = COUNTDOWN_DELAY;
        if (countdownStep < 0) state = AppState::PLAYING;
    }
}

void Game::renderCountdown() {
    std::string txt = (countdownStep > 0) ? std::to_string(countdownStep) : "FIGHT!";
    sf::Color   col = (countdownStep > 0) ? sf::Color(255, 220, 50) : sf::Color(100, 255, 100);
    drawCenteredText(window, txt, (float)WIN_HEIGHT / 2.f - 40.f, 100, col);
}

// ── PLAYING ───────────────────────────────────────────────────────────────────

void Game::updatePlaying(float dt) {
    p1.handleInput(p2);
    p2.handleInput(p1);
    p1.update(dt, world, p2);
    p2.update(dt, world, p1);
    p1.checkBulletHits(p2);
    p2.checkBulletHits(p1);

    if      (p1.isDead() && !p2.isDead()) endRound(2);
    else if (p2.isDead() && !p1.isDead()) endRound(1);
    else if (p1.isDead() &&  p2.isDead()) endRound(1);
}

void Game::renderPlaying() {
    world.draw(window);
    p1.draw(window);
    p2.draw(window);
    hud.draw(window, p1, p2, p1Wins, p2Wins);
}

// ── SCOREBOARD ────────────────────────────────────────────────────────────────

void Game::updateScoreboard(float dt) {
    bool p1Fire = sf::Keyboard::isKeyPressed(gSettings.p1.keyFire);
    bool p2Fire = sf::Keyboard::isKeyPressed(gSettings.p2.keyFire);

    if (p1Fire && !p1ReadyPrev) p1Ready = true;
    if (p2Fire && !p2ReadyPrev) p2Ready = true;
    p1ReadyPrev = p1Fire;
    p2ReadyPrev = p2Fire;

    if (p1Ready && p2Ready) startRound();
}

void Game::renderScoreboard() {
    sf::RectangleShape overlay(sf::Vector2f((float)WIN_WIDTH, (float)WIN_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    std::string winnerName = (roundWinner == 1) ? gSettings.p1.name : gSettings.p2.name;
    drawCenteredText(window, winnerName + " wins the round!",
                     140.f, 42, sf::Color(255, 220, 50));

    drawCenteredText(window,
                     std::to_string(p1Wins) + "  :  " + std::to_string(p2Wins),
                     260.f, 90);

    auto readyCol = [](bool r){ return r ? sf::Color(100, 255, 100) : sf::Color(160, 160, 160); };
    drawCenteredText(window,
        p1Ready ? gSettings.p1.name + ": READY!" : gSettings.p1.name + ": press FIRE to ready",
        440.f, 26, readyCol(p1Ready));
    drawCenteredText(window,
        p2Ready ? gSettings.p2.name + ": READY!" : gSettings.p2.name + ": press FIRE to ready",
        480.f, 26, readyCol(p2Ready));

    drawCenteredText(window, "First to 3 wins takes the match",
                     550.f, 20, sf::Color(120, 120, 120));
}

// ── WINNER SCREEN ─────────────────────────────────────────────────────────────

void Game::updateWinnerScreen(float dt) {}

void Game::renderWinnerScreen() {
    sf::Texture* bg = Assets::get().texture("menu_bg");
    if (bg) {
        sf::Sprite s(*bg);
        s.setScale((float)WIN_WIDTH  / bg->getSize().x,
                   (float)WIN_HEIGHT / bg->getSize().y);
        window.draw(s);
    }

    bool p1Won = (p1Wins >= ROUNDS_TO_WIN);
    std::string winnerName = p1Won ? gSettings.p1.name : gSettings.p2.name;
    drawCenteredText(window, winnerName + " WINS!", 80.f, 72, sf::Color(255, 220, 50));

    // Victory sprite
    std::string victoryKey = p1Won ? "p1_victory" : "p2_victory";
    sf::Texture* victoryTex = Assets::get().texture(victoryKey);
    if (victoryTex) {
        sf::Sprite vic(*victoryTex);
        sf::FloatRect vb = vic.getLocalBounds();
        vic.setOrigin(vb.width / 2.f, vb.height / 2.f);
        vic.setPosition((float)WIN_WIDTH / 2.f, (float)WIN_HEIGHT / 2.f);
        window.draw(vic);
    } else {
        sf::RectangleShape dummy(sf::Vector2f(80.f, 120.f));
        dummy.setOrigin(40.f, 60.f);
        dummy.setPosition((float)WIN_WIDTH / 2.f, (float)WIN_HEIGHT / 2.f);
        dummy.setFillColor(p1Won ? sf::Color(70, 130, 200) : sf::Color(200, 70, 70));
        window.draw(dummy);
    }

    drawCenteredText(window,
        gSettings.p1.name + "  " + std::to_string(p1Wins) +
        " - " + std::to_string(p2Wins) + "  " + gSettings.p2.name,
        (float)WIN_HEIGHT - 180.f, 26, sf::Color(180, 180, 180));

    Button btnMenu("MAIN MENU", (float)WIN_WIDTH / 2.f, (float)WIN_HEIGHT - 100.f);
    btnMenu.update(window);
    if (hasEvent && btnMenu.isClicked(currentEvent, window)) {
        p1Wins = p2Wins = 0;
        state = AppState::MAIN_MENU;
        if (menuMusicLoaded) {
            menuMusic.setVolume(gSettings.musicVolume);
            menuMusic.play();
        }
    }
    btnMenu.draw(window);
}
