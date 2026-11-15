#include <SFML/Graphics.hpp>
#include <cmath>
#include "GameConfig.hpp"
#include "BilliardBall.hpp"
#include "CueStick.hpp"
#include "PowerBar.hpp"

int main() {
    sf::RenderWindow window(
        sf::VideoMode(
            static_cast<unsigned int>(TABLE_WIDTH),
            static_cast<unsigned int>(TABLE_HEIGHT)
        ),
        "Simulasi Billiard"
    );

    window.setFramerateLimit(60);

    sf::RectangleShape woodFrame(
        sf::Vector2f(770.0f, 470.0f)
    );

    woodFrame.setPosition(15.0f, 15.0f);
    woodFrame.setFillColor(sf::Color(110, 65, 35));
    woodFrame.setOutlineThickness(3.0f);
    woodFrame.setOutlineColor(sf::Color(65, 38, 20));

    sf::RectangleShape rail(
        sf::Vector2f(730.0f, 430.0f)
    );

    rail.setPosition(35.0f, 35.0f);
    rail.setFillColor(sf::Color(25, 75, 40));

    sf::RectangleShape table(
        sf::Vector2f(690.0f, 390.0f)
    );

    table.setPosition(TABLE_LEFT, TABLE_TOP);
    table.setFillColor(sf::Color(35, 110, 60));
    table.setOutlineThickness(2.0f);
    table.setOutlineColor(sf::Color(20, 65, 35));

    BilliardBall ball(
        BALL_RADIUS,
        sf::Vector2f(400.0f, 250.0f),
        sf::Color::White
    );

    CueStick cue(150.0f, 7.0f);
    PowerBar powerBar;

    sf::Clock clock;

    bool charging = false;
    bool striking = false;
    bool ballHit = false;
    bool powerIncreasing = true;

    float power = 0.0f;
    float pullBack = 0.0f;

    float strikeTimer = 0.0f;
    float strikeDuration = 0.2f;

    sf::Vector2f direction(1.0f, 0.0f);
    sf::Vector2f shotDirection(1.0f, 0.0f);
    sf::Vector2f shotStartPosition(400.0f, 250.0f);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left &&
                    !ball.isMoving() &&
                    !striking) {

                    charging = true;
                    power = 0.0f;
                    powerIncreasing = true;
                    pullBack = 0.0f;
                }

                if (event.mouseButton.button == sf::Mouse::Right &&
                    charging) {

                    charging = false;
                    power = 0.0f;
                    powerIncreasing = true;
                    pullBack = 0.0f;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left &&
                    charging) {

                    charging = false;
                    striking = true;
                    ballHit = false;

                    strikeTimer = 0.0f;

                    shotDirection = direction;
                    shotStartPosition = ball.getPosition();

                    pullBack = power * 0.4f;
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();

        if (deltaTime > 0.05f) {
            deltaTime = 0.05f;
        }

        sf::Vector2i mousePosition =
            sf::Mouse::getPosition(window);

        sf::Vector2f mouse(
            static_cast<float>(mousePosition.x),
            static_cast<float>(mousePosition.y)
        );

        sf::Vector2f ballPosition = ball.getPosition();

        if (!ball.isMoving() && !striking) {
            sf::Vector2f directionVector = ballPosition - mouse;

            float distance = std::sqrt(
                directionVector.x * directionVector.x +
                directionVector.y * directionVector.y
            );

            if (distance > 0.0f) {
                direction = directionVector / distance;
            }
        }

        if (charging) {
            if (powerIncreasing) {
                power += 60.0f * deltaTime;

                if (power >= 100.0f) {
                    power = 100.0f;
                    powerIncreasing = false;
                }
            }
            else {
                power -= 60.0f * deltaTime;

                if (power <= 0.0f) {
                    power = 0.0f;
                    powerIncreasing = true;
                }
            }

            pullBack = power * 0.4f;
        }

        if (striking) {
            strikeTimer += deltaTime;

            float progress = strikeTimer / strikeDuration;

            if (progress < 0.5f) {
                float t = progress / 0.5f;
                pullBack = power * 0.4f * (1.0f - t);
            }
            else {
                pullBack = 0.0f;

                if (!ballHit) {
                    ball.hit(shotDirection, power);
                    ballHit = true;
                }
            }

            if (progress >= 1.0f) {
                striking = false;
                power = 0.0f;
                powerIncreasing = true;
                pullBack = 0.0f;
            }
        }

        if (striking) {
            cue.update(
                shotStartPosition,
                shotDirection,
                pullBack
            );
        }
        else if (!ball.isMoving()) {
            cue.update(
                ball.getPosition(),
                direction,
                pullBack
            );
        }

        ball.update(
            deltaTime,
            BALL_LEFT,
            BALL_RIGHT,
            BALL_TOP,
            BALL_BOTTOM
        );

        window.clear(sf::Color(35, 25, 18));

        window.draw(woodFrame);
        window.draw(rail);
        window.draw(table);

        if (!ball.isMoving() || striking) {
            cue.draw(window);
        }

        ball.draw(window);

        if ((charging || striking) && !ball.isMoving()) {
            powerBar.draw(
                window,
                ball.getPosition(),
                power
            );
        }

        window.display();
    }

    return 0;
}
