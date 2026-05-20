#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Assets.hpp"
#include "Constants.hpp"

// Simple clickable button
struct Button {
    sf::RectangleShape box;
    sf::Text           label;

    Button(const std::string& text, float x, float y,
           float w = 260.f, float h = 55.f)
    {
        box.setSize(sf::Vector2f(w, h));
        box.setOrigin(w / 2.f, h / 2.f);
        box.setPosition(x, y);
        box.setFillColor(sf::Color(50, 50, 70));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(100, 100, 140));

        if (sf::Font* f = Assets::get().font()) {
            label.setFont(*f);
            label.setString(text);
            label.setCharacterSize(24);
            label.setFillColor(sf::Color::White);
            sf::FloatRect lb = label.getLocalBounds();
            label.setOrigin(lb.width / 2.f, lb.height / 2.f + lb.top);
            label.setPosition(x, y);
        }
    }

    void update(const sf::RenderWindow& window) {
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        bool hovered = box.getGlobalBounds().contains(mouse);
        box.setFillColor(hovered ? sf::Color(80, 80, 120) : sf::Color(50, 50, 70));
    }

    bool isClicked(const sf::Event& e, const sf::RenderWindow& window) {
        if (e.type != sf::Event::MouseButtonReleased) return false;
        if (e.mouseButton.button != sf::Mouse::Left)  return false;
        sf::Vector2f m = window.mapPixelToCoords(
            sf::Vector2i(e.mouseButton.x, e.mouseButton.y));
        return box.getGlobalBounds().contains(m);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(box);
        window.draw(label);
    }
};

inline void drawCenteredText(sf::RenderWindow& window,
                              const std::string& str,
                              float y,
                              unsigned int size = 48,
                              sf::Color color = sf::Color::White)
{
    sf::Font* f = Assets::get().font();
    if (!f) return;
    sf::Text t;
    t.setFont(*f);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(color);
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.width / 2.f, b.height / 2.f);
    t.setPosition((float)WIN_WIDTH / 2.f, y);
    window.draw(t);
}
