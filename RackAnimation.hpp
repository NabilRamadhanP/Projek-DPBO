#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"

class RackAnimation {
public:
    explicit RackAnimation(const sf::Texture& triangleTex);

    void start(const std::vector<Ball*>& balls, const std::vector<sf::Vector2f>& slots);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool isFinished() const;

    static std::vector<int> makeOrder();
    static std::vector<sf::Vector2f> makeSlots(sf::Vector2f apex);

private:
    struct Move {
        Ball* ball;
        sf::Vector2f from;
        sf::Vector2f to;
        float delay;
    };

    std::vector<Move> moves;
    sf::Sprite triangle;
    float elapsed;
    float total;
    bool finished;
};