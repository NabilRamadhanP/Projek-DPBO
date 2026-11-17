#include "BilliardBall.hpp"
#include <cmath>

BilliardBall::BilliardBall(float x, float y, float radius, sf::Color color, bool isCue)
    : radius(radius), mass(1.0f), isCueBall(isCue), active(true), initialPosition(x, y) {
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(x, y);
    shape.setFillColor(color);
    shape.setOutlineThickness(1.5f);
    shape.setOutlineColor(sf::Color(30, 30, 30));
    velocity = sf::Vector2f(0.f, 0.f);
}

void BilliardBall::update(float dt, float friction) {
    if (!active) return;

    shape.move(velocity * dt);

    float damping = std::pow(friction, dt * 60.f);
    velocity *= damping;

    float speed = std::hypot(velocity.x, velocity.y);
    if (speed < 5.0f) {
        velocity = sf::Vector2f(0.f, 0.f);
    }
}

void BilliardBall::checkWallCollision(const sf::FloatRect& tableBounds, float restitution) {
    if (!active) return;

    sf::Vector2f pos = shape.getPosition();
    float left = tableBounds.left + radius;
    float right = tableBounds.left + tableBounds.width - radius;
    float top = tableBounds.top + radius;
    float bottom = tableBounds.top + tableBounds.height - radius;

    if (pos.x < left) {
        pos.x = left;
        velocity.x = -velocity.x * restitution;
    }
    else if (pos.x > right) {
        pos.x = right;
        velocity.x = -velocity.x * restitution;
    }

    if (pos.y < top) {
        pos.y = top;
        velocity.y = -velocity.y * restitution;
    }
    else if (pos.y > bottom) {
        pos.y = bottom;
        velocity.y = -velocity.y * restitution;
    }

    shape.setPosition(pos);
}

void BilliardBall::checkBallCollision(BilliardBall& other, float restitution) {
    if (!active || !other.active || this == &other) return;

    sf::Vector2f pos1 = shape.getPosition();
    sf::Vector2f pos2 = other.shape.getPosition();
    sf::Vector2f delta = pos2 - pos1;
    float dist = std::hypot(delta.x, delta.y);
    float minDist = radius + other.radius;

    // Cek apakah kedua bola saling bersentuhan/tumpang tindih
    if (dist > 0.f && dist < minDist) {
        sf::Vector2f normal = delta / dist;

        float overlap = 0.5f * (minDist - dist);
        pos1 -= normal * overlap;
        pos2 += normal * overlap;
        shape.setPosition(pos1);
        other.shape.setPosition(pos2);

        sf::Vector2f relVel = other.velocity - velocity;
        float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

        if (velAlongNormal > 0) return;

        float impulse = -(1.0f + restitution) * velAlongNormal;
        impulse /= (1.0f / mass + 1.0f / other.mass);

        sf::Vector2f impulseVec = impulse * normal;
        velocity -= (1.0f / mass) * impulseVec;
        other.velocity += (1.0f / other.mass) * impulseVec;
    }
}

bool BilliardBall::checkPocketCollision(const std::vector<sf::Vector2f>& pockets, float pocketRadius) {
    if (!active) return false;

    sf::Vector2f pos = shape.getPosition();
    for (const auto& pocket : pockets) {
        float dist = std::hypot(pos.x - pocket.x, pos.y - pocket.y);
        if (dist <= pocketRadius * 0.9f) {
            if (isCueBall) {
                resetPosition();
            }
            else {
                active = false;
                velocity = sf::Vector2f(0.f, 0.f);
            }
            return true;
        }
    }
    return false;
}

void BilliardBall::resetPosition() {
    shape.setPosition(initialPosition);
    velocity = sf::Vector2f(0.f, 0.f);
    active = true;
}

void BilliardBall::setVelocity(const sf::Vector2f& vel) { velocity = vel; }
sf::Vector2f BilliardBall::getVelocity() const { return velocity; }
sf::Vector2f BilliardBall::getPosition() const { return shape.getPosition(); }
float BilliardBall::getRadius() const { return radius; }
bool BilliardBall::isMoving() const { return active && (std::hypot(velocity.x, velocity.y) > 0.f); }
bool BilliardBall::isActive() const { return active; }
bool BilliardBall::getIsCueBall() const { return isCueBall; }

void BilliardBall::draw(sf::RenderWindow& window) {
    if (active) {
        window.draw(shape);
    }
}