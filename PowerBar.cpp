#include "PowerBar.hpp"
#include "GameConfig.hpp"
#include <cmath>

PowerBar::PowerBar(sf::Vector2f pos, sf::Vector2f size) : phase(0.f), ratio(0.f) {
    back.setPosition(pos);
    back.setSize(size);
    back.setFillColor(sf::Color(40, 40, 40));
    back.setOutlineThickness(2.f);
    back.setOutlineColor(sf::Color::White);

    fill.setPosition(pos);
    fill.setSize(sf::Vector2f(0.f, size.y));
}

void PowerBar::start() {
    reset();
    update(0.f);
}

void PowerBar::update(float dt) {
    phase = std::fmod(phase + dt * POWER_SPEED, 2.f);
    ratio = phase <= 1.f ? phase : 2.f - phase;

    fill.setSize(sf::Vector2f(back.getSize().x * ratio, back.getSize().y));
    if (ratio < 0.5f) fill.setFillColor(sf::Color::Green);
    else if (ratio < 0.8f) fill.setFillColor(sf::Color::Yellow);
    else fill.setFillColor(sf::Color::Red);
}

void PowerBar::reset() {
    phase = 0.f;
    ratio = 0.f;
    fill.setSize(sf::Vector2f(0.f, back.getSize().y));
}

float PowerBar::getRatio() const { return ratio; }

void PowerBar::draw(sf::RenderWindow& window) const {
    window.draw(back);
    window.draw(fill);
}