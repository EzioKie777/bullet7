#include "HUD.hpp"
#include "Player.hpp"
#include "Constants.hpp"
#include "Assets.hpp"
#include "Settings.hpp"
#include <string>

void HUD::draw(sf::RenderWindow& window,
               const Player& p1, const Player& p2,
               int p1Wins, int p2Wins)
{
    drawAmmoPips(window, 20.f, 20.f,
                 p1.getAmmo(), false,
                 p1.getGunState() == GunState::RELOADING);

    drawAmmoPips(window, (float)WIN_WIDTH - 20.f, 20.f,
                 p2.getAmmo(), true,
                 p2.getGunState() == GunState::RELOADING);

    drawScore(window, p1Wins, p2Wins,
              gSettings.p1.name, gSettings.p2.name);
}

void HUD::drawAmmoPips(sf::RenderWindow& window,
                       float x, float y, int ammo,
                       bool flipSide, bool reloading)
{
    const float PIP_R   = 7.f;
    const float SPACING = 20.f;

    for (int i = 0; i < MAX_AMMO; i++) {
        float px = flipSide
            ? x - i * SPACING - PIP_R
            : x + i * SPACING + PIP_R;

        sf::Texture* tex = Assets::get().texture(i < ammo ? "ammo_full" : "ammo_empty");
        if (tex) {
            sf::Sprite pip(*tex);
            sf::FloatRect lb = pip.getLocalBounds();
            pip.setOrigin(lb.width / 2.f, lb.height / 2.f);
            pip.setPosition(px, y + PIP_R);
            window.draw(pip);
        } else {
            sf::CircleShape pip(PIP_R);
            pip.setOrigin(PIP_R, PIP_R);
            pip.setPosition(px, y + PIP_R);

            if (reloading) {
                pip.setFillColor(sf::Color(60, 60, 60));
                pip.setOutlineThickness(1.f);
                pip.setOutlineColor(sf::Color(200, 100, 50));
            } else if (i < ammo) {
                pip.setFillColor(sf::Color(255, 220, 50));
            } else {
                pip.setFillColor(sf::Color(50, 50, 50));
                pip.setOutlineThickness(1.f);
                pip.setOutlineColor(sf::Color(100, 100, 100));
            }
            window.draw(pip);
        }
    }

    if (reloading) {
        if (sf::Font* f = Assets::get().font()) {
            sf::Text txt;
            txt.setFont(*f);
            txt.setString("RELOADING...");
            txt.setCharacterSize(11);
            txt.setFillColor(sf::Color(200, 100, 50));
            txt.setPosition(
                flipSide ? x - MAX_AMMO * SPACING : x,
                y + PIP_R * 2.f + 4.f);
            window.draw(txt);
        }
    }
}

void HUD::drawScore(sf::RenderWindow& window,
                    int p1Wins, int p2Wins,
                    const std::string& p1Name,
                    const std::string& p2Name)
{
    sf::Font* f = Assets::get().font();
    if (!f) return;

    std::string scoreStr = std::to_string(p1Wins) + "  :  " + std::to_string(p2Wins);

    sf::Text score;
    score.setFont(*f);
    score.setString(scoreStr);
    score.setCharacterSize(36);
    score.setFillColor(sf::Color::White);
    score.setStyle(sf::Text::Bold);
    sf::FloatRect sb = score.getLocalBounds();
    score.setOrigin(sb.width / 2.f, 0.f);
    score.setPosition((float)WIN_WIDTH / 2.f, 10.f);
    window.draw(score);

    sf::Text n1;
    n1.setFont(*f); n1.setString(p1Name); n1.setCharacterSize(14);
    n1.setFillColor(sf::Color(150, 190, 255));
    n1.setPosition(
        (float)WIN_WIDTH / 2.f - sb.width / 2.f - n1.getLocalBounds().width - 10.f,
        18.f);
    window.draw(n1);

    sf::Text n2;
    n2.setFont(*f); n2.setString(p2Name); n2.setCharacterSize(14);
    n2.setFillColor(sf::Color(255, 150, 150));
    n2.setPosition((float)WIN_WIDTH / 2.f + sb.width / 2.f + 10.f, 18.f);
    window.draw(n2);
}
