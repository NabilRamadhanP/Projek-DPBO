#pragma once
#include <SFML/Graphics.hpp>
#include "GameConfig.hpp"

class Ball {
public:
    Ball(const sf::Texture& texture, sf::Vector2f pos);
    virtual ~Ball() = default;

    void update(float dt);
    void draw(sf::RenderWindow& window);
    float hitBall(Ball& other);
    void pocket();
    void place(sf::Vector2f pos);

    void setPosition(sf::Vector2f pos);
    void setVelocity(sf::Vector2f vel);
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    float getRadius() const;
    bool isOnTable() const;
    bool isMoving() const;

protected:
    virtual void onPocketed() {}

private:
    sf::Sprite sprite;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rollAngle;
    float sinkScale;
    float baseScale;
    bool pocketed;
    bool visible;
};

class CueBall : public Ball {
public:
    CueBall(const sf::Texture& texture, sf::Vector2f pos);
    bool isScratched() const;
    void clearScratch();

protected:
    void onPocketed() override;

private:
    bool scratched;
};

class ObjectBall : public Ball {
public:
    ObjectBall(const sf::Texture& texture, sf::Vector2f pos, int number);
    int getNumber() const;
    BallGroup getGroup() const;

private:
    int number;
};