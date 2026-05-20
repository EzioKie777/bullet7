#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Player;

class HUD {
public:
    void draw(sf::RenderWindow& window,
              const Player& p1, const Player& p2,
              int p1Wins, int p2Wins);

private:
    void drawAmmoPips(sf::RenderWindow& window,
                      float x, float y, int ammo,
                      bool flipSide, bool reloading);
    void drawScore(sf::RenderWindow& window,
                   int p1Wins, int p2Wins,
                   const std::string& p1Name,
                   const std::string& p2Name);
};
