#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"

class Table {
public:
    Table();

    void draw(sf::RenderWindow& window) const;
    float bounceOffCushion(Ball& ball) const;
    int pocketIndexOf(const Ball& ball) const;
    bool isInside(sf::Vector2f pos, float radius) const;
    float headStringX() const;
    sf::Vector2f headSpot() const;
    sf::Vector2f footSpot() const;
    sf::Vector2f randomPoint() const;

private:
    sf::FloatRect bounds;
    std::vector<sf::Vector2f> pockets;
    sf::RectangleShape wood;
    sf::RectangleShape felt;
    std::vector<sf::CircleShape> pocketShapes;
};