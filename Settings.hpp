#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include "Constants.hpp"

struct PlayerSettings {
    std::string           name;
    sf::Keyboard::Key     keyLeft;
    sf::Keyboard::Key     keyRight;
    sf::Keyboard::Key     keyJump;
    sf::Keyboard::Key     keyRun;
    sf::Keyboard::Key     keyFire;
    sf::Keyboard::Key     keyClimbUp;
    sf::Keyboard::Key     keyClimbDown;
};

struct Settings {
    float musicVolume = 70.f;
    float sfxVolume   = 100.f;
    bool  fullscreen  = false;

    PlayerSettings p1 = {
        DEFAULT_P1_NAME,
        sf::Keyboard::A,
        sf::Keyboard::D,
        sf::Keyboard::W,
        sf::Keyboard::LShift,
        sf::Keyboard::G,
        sf::Keyboard::W,
        sf::Keyboard::S
    };

    PlayerSettings p2 = {
        DEFAULT_P2_NAME,
        sf::Keyboard::Left,
        sf::Keyboard::Right,
        sf::Keyboard::Up,
        sf::Keyboard::RShift,
        sf::Keyboard::L,
        sf::Keyboard::Up,
        sf::Keyboard::Down
    };
};

// One global instance used everywhere
inline Settings gSettings;
