#include "CueStick.hpp"
#include <cmath>

CueStick::CueStick() : aimLine(sf::Lines, 2), direction(1.f, 0.f) {
    stick.setSize(sf::Vector2f(220.f, 7.f));
    stick.setFillColor(sf::Color(193, 154, 107));
    stick.setOutlineThickness(1.f);
    stick.setOutlineColor(sf::Color(90, 60, 30));
}

void CueStick::update(sf::Vector2f mouse, const Ball& cueBall, float pullBack) {
    sf::Vector2f ballPos = cueBall.getPosition();
    sf::Vector2f diff = mouse - ballPos;
    float len = std::hypot(diff.x, diff.y);
    if (len > 0.f) direction = diff / len;

    float angle = std::atan2(direction.y, direction.x) * 180.f / PI;
    float gap = cueBall.getRadius() + 10.f + pullBack;

    stick.setOrigin(stick.getSize().x, stick.getSize().y / 2.f);
    stick.setPosition(ballPos - direction * gap);
    stick.setRotation(angle);

    aimLine[0].position = ballPos + direction * cueBall.getRadius();
    aimLine[0].color = sf::Color(255, 255, 255, 140);
    aimLine[1].position = ballPos + direction * 180.f;
    aimLine[1].color = sf::Color(255, 255, 255, 20);
}

void CueStick::draw(sf::RenderWindow& window) const {
    window.draw(aimLine);
    window.draw(stick);
}

sf::Vector2f CueStick::getDirection() const { return direction; }