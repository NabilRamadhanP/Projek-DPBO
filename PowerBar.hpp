#pragma once
#include <SFML/Graphics.hpp>

class PowerBar {
private:
    sf::RectangleShape background;
    sf::RectangleShape fill;
    float maxPower;
    float currentPower;

public:
    PowerBar(float x, float y, float width, float height, float maxPower = 1200.f);

    void setPower(float power);
    float getPower() const;
    float getMaxPower() const;
    void reset();
    void draw(sf::RenderWindow& window);
};