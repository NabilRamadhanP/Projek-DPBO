#include "PowerBar.hpp"
#include <algorithm>

PowerBar::PowerBar(float x, float y, float width, float height, float maxPower)
    : maxPower(maxPower), currentPower(0.f) {
    background.setPosition(x, y);
    background.setSize(sf::Vector2f(width, height));
    background.setFillColor(sf::Color(40, 40, 40));
    background.setOutlineThickness(2.f);
    background.setOutlineColor(sf::Color::White);

    fill.setPosition(x, y);
    fill.setSize(sf::Vector2f(0.f, height));
    fill.setFillColor(sf::Color::Green);
}

void PowerBar::setPower(float power) {
    currentPower = std::clamp(power, 0.f, maxPower);
    float ratio = currentPower / maxPower;
    fill.setSize(sf::Vector2f(background.getSize().x * ratio, background.getSize().y));

    // Gradasi warna indikator kekuatan (Hijau -> Kuning -> Merah)
    if (ratio < 0.5f) {
        fill.setFillColor(sf::Color::Green);
    }
    else if (ratio < 0.8f) {
        fill.setFillColor(sf::Color::Yellow);
    }
    else {
        fill.setFillColor(sf::Color::Red);
    }
}

float PowerBar::getPower() const { return currentPower; }
float PowerBar::getMaxPower() const { return maxPower; }

void PowerBar::reset() {
    currentPower = 0.f;
    fill.setSize(sf::Vector2f(0.f, background.getSize().y));
}

void PowerBar::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(fill);
}