#pragma once
#include <SFML/Graphics.hpp>
#include "GameConfig.hpp"

class PowerBar {
public:
    PowerBar(sf::Vector2f pos, sf::Vector2f size);

    void start();
    void update(float dt);
    void reset();
    float getRatio() const;
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape back;
    sf::RectangleShape fill;
    float phase;
    float ratio;
};