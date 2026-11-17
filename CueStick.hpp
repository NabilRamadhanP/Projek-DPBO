#pragma once
#include <SFML/Graphics.hpp>
#include "BilliardBall.hpp"
#include "PowerBar.hpp"

class CueStick {
private:
    sf::RectangleShape stickShape;
    sf::VertexArray aimLine;
    sf::Vector2f direction;
    bool isCharging;
    bool isVisible;
    float chargeSpeed;
    float pullBackOffset;

public:
    CueStick();

    void handleEvent(const sf::Event& event, BilliardBall& cueBall, PowerBar& powerBar);
    void update(float dt, const sf::Vector2f& mousePos, const BilliardBall& cueBall, PowerBar& powerBar, bool anyBallMoving);
    void draw(sf::RenderWindow& window);
};