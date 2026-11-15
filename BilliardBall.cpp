#include "BilliardBall.hpp"
#include <cmath>

BilliardBall::BilliardBall(
    float radius,
    sf::Vector2f position,
    sf::Color color
) {
    ball.setRadius(radius);
    ball.setOrigin(radius, radius);
    ball.setPosition(position);
    ball.setFillColor(color);

    velocity = sf::Vector2f(0.0f, 0.0f);
}

void BilliardBall::update(
    float deltaTime,
    float minX,
    float maxX,
    float minY,
    float maxY
) {
    if (!isMoving()) {
        return;
    }

    ball.move(velocity * deltaTime);

    sf::Vector2f position = ball.getPosition();

    bool hitEdge = false;

    if (position.x < minX) {
        position.x = minX;
        hitEdge = true;
    }
    else if (position.x > maxX) {
        position.x = maxX;
        hitEdge = true;
    }

    if (position.y < minY) {
        position.y = minY;
        hitEdge = true;
    }
    else if (position.y > maxY) {
        position.y = maxY;
        hitEdge = true;
    }

    if (hitEdge) {
        ball.setPosition(position);
        velocity = sf::Vector2f(0.0f, 0.0f);
        return;
    }

    float speed = std::sqrt(
        velocity.x * velocity.x +
        velocity.y * velocity.y
    );

    speed -= 100.0f * deltaTime;

    if (speed <= 5.0f) {
        velocity = sf::Vector2f(0.0f, 0.0f);
    }
    else {
        float length = std::sqrt(
            velocity.x * velocity.x +
            velocity.y * velocity.y
        );

        velocity = velocity / length * speed;
    }
}

void BilliardBall::draw(sf::RenderWindow& window) const {
    window.draw(ball);
}

sf::Vector2f BilliardBall::getPosition() const {
    return ball.getPosition();
}

bool BilliardBall::isMoving() const {
    float speed = std::sqrt(
        velocity.x * velocity.x +
        velocity.y * velocity.y
    );

    return speed > 0.1f;
}

void BilliardBall::hit(
    sf::Vector2f direction,
    float power
) {
    velocity = direction * (power * 5.0f);
}
