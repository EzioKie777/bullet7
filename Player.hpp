#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include "Bullet.hpp"
#include "World.hpp"
#include "Constants.hpp"
#include "Settings.hpp"
#include "Assets.hpp"

enum class GunState  { HOLSTERED, SNAPPING_UP, SWEEPING_DOWN, RELOADING };
enum class MoveState { IDLE, RUNNING, JUMPING, ON_LADDER };

class Player {
public:
    Player(sf::Vector2f spawnPos, int playerIndex);

    // Each frame: this player reads keys → moves opponent's body, fires own gun
    void handleInput(Player& moveTarget);
    void update(float dt, const World& world, Player& opponent);
    void checkBulletHits(Player& opponent);
    void draw(sf::RenderWindow& window);

    // Called by opponent's handleInput
    void pressLeft()      { inputLeft      = true; }
    void pressRight()     { inputRight     = true; }
    void pressJump()      { inputJump      = true; }
    void pressRun()       { inputRun       = true; }
    void pressClimbUp()   { inputClimbUp   = true; }
    void pressClimbDown() { inputClimbDown = true; }

    sf::Vector2f  getCenter()   const;
    sf::FloatRect getBounds()   const { return body.getGlobalBounds(); }
    bool          isDead()      const { return dead; }
    int           getAmmo()     const { return ammo; }
    GunState      getGunState() const { return gunState; }
    std::string   getName()     const;

    void reset(sf::Vector2f spawnPos);

private:
    void applyMovement(float dt);
    void applyGravity(float dt);
    void resolveCollisions(const World& world);
    void handleLadder(float dt, const World& world);
    void clearInputFlags();

    void updateGun(float dt, Player& opponent);
    void spawnBullet(sf::Vector2f dir);
    void updateBullets(float dt, const World& world);

    float angleTo(sf::Vector2f target) const;
    static float normalizeAngle(float a);

    void updateSprite();
    void drawNametag(sf::RenderWindow& window);

    // Shapes (fallback when sprites missing)
    sf::RectangleShape body;
    sf::CircleShape    head;
    sf::RectangleShape gun;

    sf::Sprite sprite;

    int          playerIndex;
    sf::Vector2f velocity;
    bool         onGround   = false;
    bool         onLadder   = false;
    bool         facingRight= true;
    bool         dead       = false;

    MoveState moveState = MoveState::IDLE;
    GunState  gunState  = GunState::HOLSTERED;

    float gunAngle  = 90.f;   // 90 = pointing down (holstered)
    float gunTimer  = 0.f;
    int   ammo      = MAX_AMMO;
    float flashTimer= 0.f;

    // Ladder animation
    float ladderAnimTimer = 0.f;
    bool  ladderFrameA    = true;

    bool inputLeft      = false;
    bool inputRight     = false;
    bool inputJump      = false;
    bool inputRun       = false;
    bool inputClimbUp   = false;
    bool inputClimbDown = false;
    bool prevFireHeld   = false;

    std::vector<Bullet> bullets;

    sf::Sound soundGunshot;
    sf::Sound soundReload;
};
