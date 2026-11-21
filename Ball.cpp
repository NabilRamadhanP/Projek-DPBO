#include "Ball.hpp"
#include "GameConfig.hpp"
#include <algorithm>
#include <cmath>

Ball::Ball(const sf::Texture& texture, sf::Vector2f pos)
    : sprite(texture), position(pos), velocity(0.f, 0.f), rollAngle(0.f),
    sinkScale(1.f), pocketed(false), visible(true) {
    sf::Vector2u size = texture.getSize();
    baseScale = 2.f * BALL_RADIUS / size.x;
    sprite.setOrigin(size.x / 2.f, size.y / 2.f);
}

void Ball::update(float dt) {
    if (pocketed) {
        sinkScale = std::max(0.f, sinkScale - dt * 3.f);
        if (sinkScale == 0.f) visible = false;
        return;
    }
    setPosition(position + velocity * dt);
    velocity *= std::pow(FRICTION, dt * 60.f);
    if (std::hypot(velocity.x, velocity.y) < 5.f) velocity = sf::Vector2f(0.f, 0.f);
}

void Ball::draw(sf::RenderWindow& window) {
    if (!visible) return;
    float r = BALL_RADIUS * sinkScale;

    sf::CircleShape shadow(r);
    shadow.setOrigin(r, r);
    shadow.setPosition(position + sf::Vector2f(3.f, 4.f));
    shadow.setFillColor(sf::Color(0, 0, 0, 90));
    window.draw(shadow);

    sprite.setPosition(position);
    sprite.setRotation(rollAngle * 180.f / PI);
    sprite.setScale(baseScale * sinkScale, baseScale * sinkScale);
    window.draw(sprite);

    for (int i = 0; i < 3; ++i) {
        float hr = r * (0.45f - 0.13f * i);
        sf::CircleShape shine(hr);
        shine.setOrigin(hr, hr);
        shine.setPosition(position + sf::Vector2f(-0.3f * r, -0.35f * r));
        shine.setFillColor(sf::Color(255, 255, 255, 30 + 40 * i));
        window.draw(shine);
    }

    sf::CircleShape rim(r);
    rim.setOrigin(r, r);
    rim.setPosition(position);
    rim.setFillColor(sf::Color::Transparent);
    rim.setOutlineThickness(1.5f);
    rim.setOutlineColor(sf::Color(0, 0, 0, 120));
    window.draw(rim);
}

float Ball::hitBall(Ball& other) {
    sf::Vector2f delta = other.position - position;
    float dist = std::hypot(delta.x, delta.y);
    float minDist = 2.f * BALL_RADIUS;
    if (dist <= 0.f || dist >= minDist) return 0.f;

    sf::Vector2f n = delta / dist;
    float overlap = (minDist - dist) * 0.5f;
    setPosition(position - n * overlap);
    other.setPosition(other.position + n * overlap);

    sf::Vector2f relVel = other.velocity - velocity;
    float vn = relVel.x * n.x + relVel.y * n.y;
    if (vn >= 0.f) return 0.f;

    float impulse = -(1.f + BALL_RESTITUTION) * vn / 2.f;
    velocity -= impulse * n;
    other.velocity += impulse * n;
    return -vn;
}

void Ball::pocket() {
    pocketed = true;
    velocity = sf::Vector2f(0.f, 0.f);
    onPocketed();
}

void Ball::place(sf::Vector2f pos) {
    position = pos;
    velocity = sf::Vector2f(0.f, 0.f);
    pocketed = false;
    sinkScale = 1.f;
    visible = true;
}

void Ball::setPosition(sf::Vector2f pos) {
    sf::Vector2f d = pos - position;
    float dist = std::hypot(d.x, d.y);
    rollAngle += (d.x >= 0.f ? 1.f : -1.f) * dist / BALL_RADIUS;
    position = pos;
}

void Ball::setVelocity(sf::Vector2f vel) { velocity = vel; }
sf::Vector2f Ball::getPosition() const { return position; }
sf::Vector2f Ball::getVelocity() const { return velocity; }
float Ball::getRadius() const { return BALL_RADIUS; }
bool Ball::isOnTable() const { return !pocketed; }
bool Ball::isMoving() const { return !pocketed && (velocity.x != 0.f || velocity.y != 0.f); }

CueBall::CueBall(const sf::Texture& texture, sf::Vector2f pos) : Ball(texture, pos), scratched(false) {}
void CueBall::onPocketed() { scratched = true; }
bool CueBall::isScratched() const { return scratched; }
void CueBall::clearScratch() { scratched = false; }

ObjectBall::ObjectBall(const sf::Texture& texture, sf::Vector2f pos, int n) : Ball(texture, pos), number(n) {}
int ObjectBall::getNumber() const { return number; }

BallGroup ObjectBall::getGroup() const {
    if (number < 8) return BallGroup::Solid;
    if (number > 8) return BallGroup::Stripe;
    return BallGroup::None;
}