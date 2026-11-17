#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class BilliardBall {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float radius;
    float mass;
    bool isCueBall;
    bool active;
    sf::Vector2f initialPosition;

public:
    BilliardBall(float x, float y, float radius, sf::Color color, bool isCue = false);

    // Update pergerakan dan gesekan (Aktivitas 5 & 7)
    void update(float dt, float friction);

    // Deteksi dan respons tumbukan dengan dinding meja (Aktivitas 7)
    void checkWallCollision(const sf::FloatRect& tableBounds, float restitution = 0.85f);

    // Deteksi dan respons tumbukan elastis dengan bola lain (Aktivitas 7)
    void checkBallCollision(BilliardBall& other, float restitution = 0.95f);

    // Deteksi bola masuk ke dalam lubang (Aktivitas 7)
    bool checkPocketCollision(const std::vector<sf::Vector2f>& pockets, float pocketRadius);

    // Mengembalikan bola ke posisi awal (khusus bola putih jika masuk lubang)
    void resetPosition();

    // Getter & Setter
    void setVelocity(const sf::Vector2f& vel);
    sf::Vector2f getVelocity() const;
    sf::Vector2f getPosition() const;
    float getRadius() const;
    bool isMoving() const;
    bool isActive() const;
    bool getIsCueBall() const;

    void draw(sf::RenderWindow& window);
};