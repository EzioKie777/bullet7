#include "Bullet.hpp"
#include "Constants.hpp"
#include <cmath>

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir) {
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0.f) dir /= len;
    velocity = dir * BULLET_SPEED;

    shape.setRadius(4.f);
    shape.setOrigin(sf::Vector2f(4.f, 4.f));
    shape.setFillColor(sf::Color(255, 220, 50));
    shape.setPosition(pos);
}

void Bullet::update(float dt) {
    trail.push_back(shape.getPosition());
    if ((int)trail.size() > TRAIL_LEN)
        trail.erase(trail.begin());

    shape.move(velocity * dt);

    sf::Vector2f p = shape.getPosition();
    if (p.x < -50 || p.x > WIN_WIDTH  + 50 ||
        p.y < -50 || p.y > WIN_HEIGHT + 50)
        alive = false;
}

void Bullet::draw(sf::RenderWindow& window) {
    if (!alive) return;

    // Trail
    for (int i = 0; i < (int)trail.size(); i++) {
        float t      = (float)(i + 1) / trail.size();
        float radius = 3.f * t;
        sf::Uint8 alpha = (sf::Uint8)(180.f * t);

        sf::CircleShape dot(radius);
        dot.setOrigin(sf::Vector2f(radius, radius));
        dot.setPosition(trail[i]);
        dot.setFillColor(sf::Color(255, 200, 50, alpha));
        window.draw(dot);
    }

    window.draw(shape);
}
