#include <SFML/Graphics.hpp>
#include <vector>
#include "BilliardBall.hpp"
#include "CueStick.hpp"
#include "PowerBar.hpp"

int main() {
    const unsigned int WINDOW_WIDTH = 1000;
    const unsigned int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Simulasi Permainan Billiard - Kelompok 6");
    window.setFramerateLimit(60);

    // 1. Setup Meja & Dinding (Aktivitas 4)
    sf::FloatRect tableBounds(100.f, 80.f, 800.f, 420.f);
    float cushionThickness = 25.f;

    sf::RectangleShape woodFrame(sf::Vector2f(tableBounds.width + cushionThickness * 2, tableBounds.height + cushionThickness * 2));
    woodFrame.setPosition(tableBounds.left - cushionThickness, tableBounds.top - cushionThickness);
    woodFrame.setFillColor(sf::Color(101, 67, 33));

    sf::RectangleShape feltSurface(sf::Vector2f(tableBounds.width, tableBounds.height));
    feltSurface.setPosition(tableBounds.left, tableBounds.top);
    feltSurface.setFillColor(sf::Color(20, 110, 55));

    // 2. Setup 6 Lubang Meja
    float pocketRadius = 22.f;
    std::vector<sf::Vector2f> pocketPositions = {
        {tableBounds.left, tableBounds.top},
        {tableBounds.left + tableBounds.width / 2.f, tableBounds.top - 4.f},
        {tableBounds.left + tableBounds.width, tableBounds.top},
        {tableBounds.left, tableBounds.top + tableBounds.height},
        {tableBounds.left + tableBounds.width / 2.f, tableBounds.top + tableBounds.height + 4.f},
        {tableBounds.left + tableBounds.width, tableBounds.top + tableBounds.height}
    };

    std::vector<sf::CircleShape> pocketShapes;
    for (const auto& pos : pocketPositions) {
        sf::CircleShape pocket(pocketRadius);
        pocket.setOrigin(pocketRadius, pocketRadius);
        pocket.setPosition(pos);
        pocket.setFillColor(sf::Color(15, 15, 15));
        pocketShapes.push_back(pocket);
    }

    // 3. Setup Bola Putih dan Formasi Segitiga Bola Target (Aktivitas 5)
    float ballRadius = 13.f;
    std::vector<BilliardBall> balls;

    balls.emplace_back(tableBounds.left + 200.f, tableBounds.top + tableBounds.height / 2.f, ballRadius, sf::Color::White, true);

    std::vector<sf::Color> targetColors = {
        sf::Color::Yellow, sf::Color::Blue, sf::Color::Red, sf::Color::Magenta, sf::Color(255, 140, 0),
        sf::Color::Green, sf::Color(128, 0, 0), sf::Color::Black, sf::Color::Yellow, sf::Color::Blue,
        sf::Color::Red, sf::Color::Magenta, sf::Color(255, 140, 0), sf::Color::Green, sf::Color(128, 0, 0)
    };

    float startX = tableBounds.left + 540.f;
    float startY = tableBounds.top + tableBounds.height / 2.f;
    float spacingX = ballRadius * 1.78f;
    float spacingY = ballRadius * 2.05f;
    int colorIdx = 0;

    for (int col = 0; col < 5; ++col) {
        for (int row = 0; row <= col; ++row) {
            float x = startX + col * spacingX;
            float y = startY - (col * spacingY * 0.5f) + (row * spacingY);
            balls.emplace_back(x, y, ballRadius, targetColors[colorIdx % targetColors.size()], false);
            colorIdx++;
        }
    }

    // 4. Setup Tongkat dan Indikator Kekuatan (Aktivitas 6)
    CueStick cueStick;
    PowerBar powerBar(100.f, 540.f, 250.f, 22.f, 1300.f);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            cueStick.handleEvent(event, balls[0], powerBar);
        }

        // Update Fisika Gerak dan Tumbukan (Aktivitas 7)
        bool anyBallMoving = false;
        const float TABLE_FRICTION = 0.985f;

        for (size_t i = 0; i < balls.size(); ++i) {
            balls[i].update(dt, TABLE_FRICTION);
            balls[i].checkWallCollision(tableBounds);
            balls[i].checkPocketCollision(pocketPositions, pocketRadius);

            if (balls[i].isMoving()) {
                anyBallMoving = true;
            }
        }

        // Cek tumbukan antar semua pasangan bola
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                balls[i].checkBallCollision(balls[j]);
            }
        }

        // Update Tongkat berdasarkan posisi mouse
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        cueStick.update(dt, mousePos, balls[0], powerBar, anyBallMoving);

        // Render seluruh objek
        window.clear(sf::Color(30, 32, 36));
        window.draw(woodFrame);
        window.draw(feltSurface);

        for (const auto& pocket : pocketShapes) {
            window.draw(pocket);
        }

        for (auto& ball : balls) {
            ball.draw(window);
        }

        cueStick.draw(window);
        powerBar.draw(window);

        window.display();
    }

    return 0;
}