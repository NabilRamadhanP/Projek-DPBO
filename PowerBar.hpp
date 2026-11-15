#pragma once

#include <SFML/Graphics.hpp>

class PowerBar {
private:
    sf::RectangleShape background;
    sf::RectangleShape powerBar;

public:
    PowerBar();

    void draw(
        sf::RenderWindow& window,
        sf::Vector2f ballPosition,
        float power
    );
};
