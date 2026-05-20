#include "Player.hpp"
#include <cmath>
#include <algorithm>

static const float PI = 3.14159265f;

float Player::normalizeAngle(float a) {
    while (a >  180.f) a -= 360.f;
    while (a < -180.f) a += 360.f;
    return a;
}

Player::Player(sf::Vector2f spawnPos, int idx) : playerIndex(idx) {
    sf::Color bodyColor = (idx == 1)
        ? sf::Color(70, 130, 200)
        : sf::Color(200, 70, 70);

    body.setSize(sf::Vector2f(28.f, 36.f));
    body.setOrigin(14.f, 36.f);
    body.setFillColor(bodyColor);
    body.setPosition(spawnPos);

    head.setRadius(13.f);
    head.setOrigin(13.f, 13.f);
    head.setFillColor(sf::Color(220, 180, 140));

    gun.setSize(sf::Vector2f(22.f, 6.f));
    gun.setOrigin(0.f, 3.f);
    gun.setFillColor(sf::Color(60, 60, 60));

    // Try to load idle sprite
    std::string key = (idx == 1) ? "p1_idle" : "p2_idle";
    sf::Texture* tex = Assets::get().texture(key);
    if (tex) sprite.setTexture(*tex);

    // Sounds
    if (sf::SoundBuffer* b = Assets::get().sound("gunshot")) soundGunshot.setBuffer(*b);
    if (sf::SoundBuffer* b = Assets::get().sound("reload"))  soundReload.setBuffer(*b);
}

std::string Player::getName() const {
    return (playerIndex == 1) ? gSettings.p1.name : gSettings.p2.name;
}

sf::Vector2f Player::getCenter() const {
    sf::Vector2f p = body.getPosition();
    return sf::Vector2f(p.x, p.y - 18.f);
}

float Player::angleTo(sf::Vector2f target) const {
    sf::Vector2f d = target - gun.getPosition();
    return std::atan2(d.y, d.x) * 180.f / PI;
}

void Player::reset(sf::Vector2f spawnPos) {
    body.setPosition(spawnPos);
    velocity      = sf::Vector2f(0.f, 0.f);
    onGround      = false;
    onLadder      = false;
    dead          = false;
    facingRight   = (playerIndex == 1);
    moveState     = MoveState::IDLE;
    gunState      = GunState::HOLSTERED;
    gunAngle      = 90.f;
    gunTimer      = 0.f;
    ammo          = MAX_AMMO;
    flashTimer    = 0.f;
    prevFireHeld  = false;
    bullets.clear();
    clearInputFlags();
}

// ── Input ─────────────────────────────────────────────────────────────────────

void Player::handleInput(Player& moveTarget) {
    PlayerSettings& keys = (playerIndex == 1) ? gSettings.p1 : gSettings.p2;

    if (sf::Keyboard::isKeyPressed(keys.keyLeft))      moveTarget.pressLeft();
    if (sf::Keyboard::isKeyPressed(keys.keyRight))     moveTarget.pressRight();
    if (sf::Keyboard::isKeyPressed(keys.keyJump))      { moveTarget.pressJump(); moveTarget.pressClimbUp(); }
    if (sf::Keyboard::isKeyPressed(keys.keyRun))       moveTarget.pressRun();
    if (sf::Keyboard::isKeyPressed(keys.keyClimbDown)) moveTarget.pressClimbDown();

    bool fireNow = sf::Keyboard::isKeyPressed(keys.keyFire);
    if (fireNow && !prevFireHeld &&
        gunState == GunState::HOLSTERED && ammo > 0 && !dead)
        gunState = GunState::SNAPPING_UP;
    prevFireHeld = fireNow;
}

// ── Update ────────────────────────────────────────────────────────────────────

void Player::update(float dt, const World& world, Player& opponent) {
    if (dead) {
        if (flashTimer > 0.f) flashTimer -= dt;
        return;
    }

    handleLadder(dt, world);

    if (!onLadder) {
        applyMovement(dt);
        applyGravity(dt);
        body.move(velocity * dt);
        resolveCollisions(world);
    }

    clearInputFlags();

    // Sync head and gun to body
    sf::Vector2f pos = body.getPosition();
    head.setPosition(pos.x, pos.y - 36.f - 13.f);
    gun.setPosition(pos.x, pos.y - 22.f);

    updateGun(dt, opponent);
    updateBullets(dt, world);

    if (flashTimer > 0.f) flashTimer -= dt;
}

void Player::applyMovement(float dt) {
    float speed = inputRun ? RUN_SPEED : WALK_SPEED;
    velocity.x = 0.f;

    if (inputLeft)  { velocity.x = -speed; facingRight = false; }
    if (inputRight) { velocity.x =  speed; facingRight = true;  }

    if (inputJump && onGround) {
        velocity.y = JUMP_FORCE;
        onGround   = false;
        moveState  = MoveState::JUMPING;
    }

    if      (!onGround)        moveState = MoveState::JUMPING;
    else if (velocity.x != 0.f) moveState = MoveState::RUNNING;
    else                        moveState = MoveState::IDLE;
}

void Player::applyGravity(float dt) {
    if (!onGround)
        velocity.y += GRAVITY * dt;
}

void Player::resolveCollisions(const World& world) {
    onGround = false;

    for (const auto& platform : world.getPlatforms()) {
        sf::FloatRect pRect = platform.getBounds();
        sf::FloatRect bRect = body.getGlobalBounds();
        if (!bRect.intersects(pRect)) continue;

        float overlapLeft   = (bRect.left + bRect.width) - pRect.left;
        float overlapRight  = (pRect.left + pRect.width) - bRect.left;
        float overlapTop    = (bRect.top  + bRect.height) - pRect.top;
        float overlapBottom = (pRect.top  + pRect.height) - bRect.top;

        float minX = std::min(overlapLeft, overlapRight);
        float minY = std::min(overlapTop,  overlapBottom);

        if (minY < minX) {
            if (overlapTop < overlapBottom) {
                body.move(0.f, -overlapTop);
                velocity.y = 0.f;
                onGround   = true;
                if (moveState == MoveState::JUMPING) moveState = MoveState::IDLE;
            } else {
                body.move(0.f, overlapBottom);
                velocity.y = 0.f;
            }
        } else {
            if (overlapLeft < overlapRight) body.move(-overlapLeft, 0.f);
            else                            body.move( overlapRight, 0.f);
            velocity.x = 0.f;
        }
    }
}

void Player::handleLadder(float dt, const World& world) {
    sf::Vector2f pos = body.getPosition();

    const Ladder* nearLadder = nullptr;
    for (const auto& l : world.getLadders()) {
        if (l.isNear(pos.x, pos.y)) { nearLadder = &l; break; }
    }

    if (!nearLadder) { onLadder = false; return; }

    if ((inputClimbUp || inputClimbDown) && !onLadder)
        onLadder = true;

    if (!onLadder) return;

    velocity = sf::Vector2f(0.f, 0.f);
    moveState = MoveState::ON_LADDER;

    if (inputClimbUp)   body.move(0.f, -LADDER_SPEED * dt);
    if (inputClimbDown) body.move(0.f,  LADDER_SPEED * dt);

    // Clamp to ladder bounds
    sf::Vector2f p = body.getPosition();
    if (p.y < nearLadder->top)    { body.setPosition(p.x, nearLadder->top);    onLadder = false; }
    if (p.y > nearLadder->bottom) { body.setPosition(p.x, nearLadder->bottom); onLadder = false; onGround = true; }

    // Ladder climbing animation
    if (inputClimbUp || inputClimbDown) {
        ladderAnimTimer += dt;
        if (ladderAnimTimer > 0.2f) { ladderAnimTimer = 0.f; ladderFrameA = !ladderFrameA; }
    }

    if (inputJump) { onLadder = false; velocity.y = JUMP_FORCE; }
}

void Player::clearInputFlags() {
    inputLeft = inputRight = inputJump = inputRun =
    inputClimbUp = inputClimbDown = false;
}

// ── Gun ───────────────────────────────────────────────────────────────────────

void Player::updateGun(float dt, Player& opponent) {
    float targetAngle = angleTo(opponent.getCenter());

    switch (gunState) {

        case GunState::HOLSTERED:
            gunAngle = 90.f;  // pointing down at hip
            break;

        case GunState::SNAPPING_UP: {
            float diff = normalizeAngle(-90.f - gunAngle);
            float step = GUN_SNAP_SPEED * dt;
            if (std::abs(diff) <= step) {
                gunAngle = -90.f;
                gunState = GunState::SWEEPING_DOWN;
            } else {
                gunAngle += (diff > 0 ? step : -step);
            }
            break;
        }

        case GunState::SWEEPING_DOWN: {
            float diff = normalizeAngle(targetAngle - gunAngle);
            float step = GUN_SWEEP_SPEED * dt;
            if (std::abs(diff) <= GUN_LOCK_THRESHOLD) {
                // LOCKED — FIRE
                float rad = gunAngle * PI / 180.f;
                spawnBullet(sf::Vector2f(std::cos(rad), std::sin(rad)));
                ammo--;
                soundGunshot.play();

                if (ammo <= 0) {
                    gunState = GunState::RELOADING;
                    gunTimer = RELOAD_TIME;
                } else {
                    gunState = GunState::HOLSTERED;
                }
            } else {
                gunAngle += (diff > 0 ? step : -step);
            }
            break;
        }

        case GunState::RELOADING:
            gunAngle  = 90.f;
            gunTimer -= dt;
            if (gunTimer <= 0.f) {
                ammo     = MAX_AMMO;
                gunState = GunState::HOLSTERED;
                soundReload.play();
            }
            break;
    }

    gun.setRotation(gunAngle);
}

void Player::spawnBullet(sf::Vector2f dir) {
    sf::Vector2f spawnPos = gun.getPosition() + dir * 26.f;
    bullets.emplace_back(spawnPos, dir);
}

void Player::updateBullets(float dt, const World& world) {
    for (auto& b : bullets) {
        b.update(dt);
        for (const auto& p : world.getPlatforms())
            if (b.getBounds().intersects(p.getBounds()))
                b.kill();
    }
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const Bullet& b){ return !b.isAlive(); }),
        bullets.end());
}

void Player::checkBulletHits(Player& opponent) {
    if (opponent.isDead()) return;
    for (auto& b : bullets) {
        if (!b.isAlive()) continue;
        if (b.getBounds().intersects(opponent.getBounds())) {
            b.kill();
            opponent.dead       = true;
            opponent.flashTimer = HIT_FLASH_TIME;
        }
    }
}

// ── Draw ──────────────────────────────────────────────────────────────────────

void Player::updateSprite() {
    std::string key = (playerIndex == 1) ? "p1_" : "p2_";

    if (gunState == GunState::SNAPPING_UP || gunState == GunState::SWEEPING_DOWN)
        key += "windup";
    else if (gunState == GunState::RELOADING)
        key += "reload";
    else if (onLadder)
        key += (ladderFrameA ? "idle" : "jump");
    else {
        switch (moveState) {
            case MoveState::RUNNING:  key += "run";  break;
            case MoveState::JUMPING:  key += "jump"; break;
            default:                  key += "idle"; break;
        }
    }

    sf::Texture* tex = Assets::get().texture(key);
    if (tex) {
        sprite.setTexture(*tex, true);
        sprite.setOrigin(tex->getSize().x / 2.f, (float)tex->getSize().y);
        sprite.setPosition(body.getPosition());
        sprite.setScale(facingRight ? 1.f : -1.f, 1.f);
    }
}

void Player::drawNametag(sf::RenderWindow& window) {
    sf::Font* f = Assets::get().font();
    if (!f) return;

    sf::Text tag;
    tag.setFont(*f);
    tag.setString(getName());
    tag.setCharacterSize(13);
    tag.setFillColor(sf::Color::White);

    sf::FloatRect tb = tag.getLocalBounds();
    sf::Vector2f  pos = body.getPosition();
    tag.setOrigin(tb.width / 2.f, tb.height);
    tag.setPosition(pos.x, pos.y - 36.f - 26.f - 8.f);
    window.draw(tag);
}

void Player::draw(sf::RenderWindow& window) {
    if (dead && flashTimer <= 0.f) return;

    updateSprite();

    bool showFlash = (flashTimer > 0.f);
    std::string idleKey = (playerIndex == 1) ? "p1_idle" : "p2_idle";
    bool hasSprite = (Assets::get().texture(idleKey) != nullptr);

    if (hasSprite) {
        sprite.setColor(showFlash ? sf::Color(255, 100, 100) : sf::Color::White);
        window.draw(sprite);
    } else {
        sf::Color col = showFlash
            ? sf::Color::White
            : (playerIndex == 1 ? sf::Color(70, 130, 200) : sf::Color(200, 70, 70));
        body.setFillColor(col);
        head.setFillColor(showFlash ? sf::Color::White : sf::Color(220, 180, 140));
        window.draw(body);
        window.draw(head);
    }

    if (!dead) window.draw(gun);

    for (auto& b : bullets) b.draw(window);

    drawNametag(window);
}
