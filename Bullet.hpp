#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    bool isAlive() const { return alive; }
    void kill()         { alive = false; }

private:
    sf::CircleShape           shape;
    sf::Vector2f              velocity;
    std::vector<sf::Vector2f> trail;
    bool                      alive = true;

    static const int TRAIL_LEN = 8;
};
