#include "CueStick.hpp"
#include "GameConfig.hpp"
#include <cmath>

CueStick::CueStick(
    float length,
    float width
) {
    stick.setSize(sf::Vector2f(length, width));
    stick.setOrigin(length, width / 2.0f);
    stick.setFillColor(sf::Color(180, 120, 60));

    tip.setSize(sf::Vector2f(12.0f, width));
    tip.setOrigin(12.0f, width / 2.0f);
    tip.setFillColor(sf::Color(230, 200, 145));
}

void CueStick::update(
    sf::Vector2f ballPosition,
    sf::Vector2f direction,
    float pullBack
) {
    sf::Vector2f position =
        ballPosition - direction * (BALL_RADIUS + pullBack);

    float angle = std::atan2(
        direction.y,
        direction.x
    ) * 180.0f / PI;

    stick.setPosition(position);
    stick.setRotation(angle);

    tip.setPosition(position);
    tip.setRotation(angle);
}

void CueStick::draw(sf::RenderWindow& window) const {
    window.draw(stick);
    window.draw(tip);
}
