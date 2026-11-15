#pragma once

#include <SFML/Graphics.hpp>

class CueStick {
private:
    sf::RectangleShape stick;
    sf::RectangleShape tip;

public:
    CueStick(float length, float width);

    void update(
        sf::Vector2f ballPosition,
        sf::Vector2f direction,
        float pullBack
    );

    void draw(sf::RenderWindow& window) const;
};
