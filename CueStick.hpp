#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.hpp"

class CueStick {
public:
    CueStick();

    void update(sf::Vector2f mouse, const Ball& cueBall, float pullBack);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2f getDirection() const;

private:
    sf::RectangleShape stick;
    sf::VertexArray aimLine;
    sf::Vector2f direction;
};