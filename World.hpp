#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

struct Platform {
    sf::RectangleShape shape;

    Platform(float x, float y, float w, float h,
             sf::Color color = sf::Color(80, 80, 100))
    {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(w, h));
        shape.setFillColor(color);
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color(120, 120, 150));
    }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
};

struct Ladder {
    sf::RectangleShape shape;
    float x;       // center x
    float top;     // top y
    float bottom;  // bottom y

    Ladder(float cx, float topY, float bottomY)
        : x(cx), top(topY), bottom(bottomY)
    {
        shape.setSize(sf::Vector2f(16.f, bottomY - topY));
        shape.setPosition(cx - 8.f, topY);
        shape.setFillColor(sf::Color(160, 120, 60, 180));
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color(200, 160, 80));
    }

    bool isNear(float px, float py) const {
        return std::abs(px - x) < 20.f && py >= top && py <= bottom;
    }
};

class World {
public:
    World();
    void draw(sf::RenderWindow& window);

    const std::vector<Platform>& getPlatforms() const { return platforms; }
    const std::vector<Ladder>&   getLadders()   const { return ladders; }

private:
    void buildMap();

    std::vector<Platform> platforms;
    std::vector<Ladder>   ladders;
};
