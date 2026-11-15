#include "PowerBar.hpp"
#include "GameConfig.hpp"

PowerBar::PowerBar() {
    background.setSize(sf::Vector2f(100.0f, 14.0f));
    background.setFillColor(sf::Color(40, 40, 40));
    background.setOutlineThickness(1.0f);
    background.setOutlineColor(sf::Color::White);

    powerBar.setSize(sf::Vector2f(0.0f, 10.0f));
}

void PowerBar::draw(
    sf::RenderWindow& window,
    sf::Vector2f ballPosition,
    float power
) {
    float x = ballPosition.x + 25.0f;
    float y = ballPosition.y - 35.0f;

    if (x + 100.0f > TABLE_RIGHT) {
        x = ballPosition.x - 125.0f;
    }

    if (y < TABLE_TOP) {
        y = ballPosition.y + 25.0f;
    }

    sf::Vector2f position(x, y);

    background.setPosition(position);
    powerBar.setPosition(
        position.x + 2.0f,
        position.y + 2.0f
    );

    powerBar.setSize(
        sf::Vector2f(power * 0.96f, 10.0f)
    );

    if (power < 40.0f) {
        powerBar.setFillColor(sf::Color(70, 200, 90));
    }
    else if (power < 70.0f) {
        powerBar.setFillColor(sf::Color(230, 190, 50));
    }
    else {
        powerBar.setFillColor(sf::Color(220, 60, 50));
    }

    window.draw(background);
    window.draw(powerBar);
}
