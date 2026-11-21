#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button(const sf::Font& font, const std::string& label, sf::Vector2f pos, sf::Vector2f size)
        : text(label, font, 26) {
        box.setPosition(pos);
        box.setSize(size);
        box.setOutlineThickness(3.f);
        box.setOutlineColor(sf::Color::White);
        box.setFillColor(normalColor);

        sf::FloatRect b = text.getLocalBounds();
        text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        text.setPosition(pos + size / 2.f);
    }

    void update(sf::Vector2f mouse) {
        box.setFillColor(contains(mouse) ? hoverColor : normalColor);
    }

    bool contains(sf::Vector2f p) const {
        return box.getGlobalBounds().contains(p);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(box);
        window.draw(text);
    }

private:
    sf::RectangleShape box;
    sf::Text text;
    const sf::Color normalColor = sf::Color(40, 40, 40);
    const sf::Color hoverColor = sf::Color(80, 80, 80);
};