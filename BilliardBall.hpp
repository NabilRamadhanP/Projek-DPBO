#pragma once

#include <SFML/Graphics.hpp>

class BilliardBall {
private:
    sf::CircleShape ball;
    sf::Vector2f velocity;

public:
    BilliardBall(float radius, sf::Vector2f position, sf::Color color);

    void update(
        float deltaTime,
        float minX,
        float maxX,
        float minY,
        float maxY
    );

    void draw(sf::RenderWindow& window) const;

    sf::Vector2f getPosition() const;
    bool isMoving() const;

    void hit(sf::Vector2f direction, float power);
};
