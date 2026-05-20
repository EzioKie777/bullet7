#include "World.hpp"
#include "Assets.hpp"
#include "Constants.hpp"

static const sf::Color FLOOR_COLOR = sf::Color(70,  70,  90);
static const sf::Color WALL_COLOR  = sf::Color(90,  50,  50);
static const sf::Color PLAT_COLOR  = sf::Color(80,  80, 100);

World::World() {
    buildMap();
}

void World::buildMap() {
    // Ground
    platforms.emplace_back(  0.f, 660.f, 1280.f,  60.f, FLOOR_COLOR);

    // Outer walls
    platforms.emplace_back(  0.f,   0.f,   20.f, 720.f, WALL_COLOR);
    platforms.emplace_back(1260.f,  0.f,   20.f, 720.f, WALL_COLOR);

    // Mid floor — left and right rooms
    platforms.emplace_back( 20.f, 420.f,  380.f,  18.f, FLOOR_COLOR);
    platforms.emplace_back(880.f, 420.f,  380.f,  18.f, FLOOR_COLOR);

    // Left room walls
    platforms.emplace_back( 20.f, 420.f,   18.f, 240.f, WALL_COLOR);
    platforms.emplace_back(380.f, 420.f,   18.f, 240.f, WALL_COLOR);

    // Right room walls
    platforms.emplace_back(880.f, 420.f,   18.f, 240.f, WALL_COLOR);
    platforms.emplace_back(1260.f,420.f,   18.f, 240.f, WALL_COLOR);

    // Center open platforms
    platforms.emplace_back(440.f, 300.f,  160.f,  18.f, PLAT_COLOR);
    platforms.emplace_back(300.f, 390.f,  120.f,  18.f, PLAT_COLOR);
    platforms.emplace_back(860.f, 390.f,  120.f,  18.f, PLAT_COLOR);

    // Interior shelves
    platforms.emplace_back( 80.f, 530.f,  120.f,  14.f, PLAT_COLOR);
    platforms.emplace_back(220.f, 570.f,  100.f,  14.f, PLAT_COLOR);
    platforms.emplace_back(940.f, 530.f,  120.f,  14.f, PLAT_COLOR);
    platforms.emplace_back(1080.f,570.f,  100.f,  14.f, PLAT_COLOR);

    // Ladders: (centerX, topY, bottomY)
    ladders.emplace_back(200.f,  420.f, 660.f);
    ladders.emplace_back(1080.f, 420.f, 660.f);
    ladders.emplace_back(520.f,  300.f, 420.f);
}

void World::draw(sf::RenderWindow& window) {
    // Background
    sf::Texture* bgTex = Assets::get().texture("background");
    if (bgTex) {
        sf::Sprite bg(*bgTex);
        bg.setScale(
            WIN_WIDTH  / (float)bgTex->getSize().x,
            WIN_HEIGHT / (float)bgTex->getSize().y);
        window.draw(bg);
    } else {
        sf::RectangleShape bg(sf::Vector2f((float)WIN_WIDTH, (float)WIN_HEIGHT));
        bg.setFillColor(sf::Color(25, 20, 30));
        window.draw(bg);
    }

    for (auto& l : ladders)   window.draw(l.shape);
    for (auto& p : platforms) window.draw(p.shape);
}
