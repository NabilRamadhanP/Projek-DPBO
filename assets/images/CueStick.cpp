#include "CueStick.hpp"
#include <cmath>

CueStick::CueStick()
    : direction(1.f, 0.f), isCharging(false), isVisible(true), chargeSpeed(900.f), pullBackOffset(0.f) {
    stickShape.setSize(sf::Vector2f(220.f, 7.f));
    stickShape.setFillColor(sf::Color(193, 154, 107));
    stickShape.setOutlineThickness(1.f);
    stickShape.setOutlineColor(sf::Color(90, 60, 30));

    aimLine.setPrimitiveType(sf::Lines);
    aimLine.resize(2);
}

void CueStick::handleEvent(const sf::Event& event, BilliardBall& cueBall, PowerBar& powerBar) {
    if (!isVisible) return;

    // Langkah 3 Use Case: Pemain menahan klik kiri untuk mengatur kekuatan
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        isCharging = true;
        powerBar.reset();
    }

    // Alur Alternatif Use Case: Klik kanan membatalkan pukulan
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        if (isCharging) {
            isCharging = false;
            pullBackOffset = 0.f;
            powerBar.reset();
        }
    }

    // Langkah 5 Use Case: Pemain melepas klik kiri untuk memukul bola putih
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        if (isCharging) {
            float power = powerBar.getPower();
            if (power > 20.f) {
                cueBall.setVelocity(direction * power);
            }
            isCharging = false;
            pullBackOffset = 0.f;
            powerBar.reset();
        }
    }
}

void CueStick::update(float dt, const sf::Vector2f& mousePos, const BilliardBall& cueBall, PowerBar& powerBar, bool anyBallMoving) {
    // Tongkat hanya tampil saat semua bola berhenti
    isVisible = !anyBallMoving && cueBall.isActive();
    if (!isVisible) {
        isCharging = false;
        return;
    }

    sf::Vector2f ballPos = cueBall.getPosition();
    sf::Vector2f diff = mousePos - ballPos;
    float length = std::hypot(diff.x, diff.y);

    if (length > 0.f) {
        direction = diff / length;
    }

    // Tambah kekuatan selama klik kiri ditahan
    if (isCharging) {
        float newPower = powerBar.getPower() + chargeSpeed * dt;
        powerBar.setPower(newPower);
        pullBackOffset = (powerBar.getPower() / powerBar.getMaxPower()) * 45.f;
    }
    else {
        pullBackOffset = 0.f;
    }

    float angleRad = std::atan2(direction.y, direction.x);
    float angleDeg = angleRad * 180.f / 3.14159265f;

    float gapFromBall = cueBall.getRadius() + 10.f + pullBackOffset;
    sf::Vector2f stickPos = ballPos - direction * gapFromBall;

    stickShape.setOrigin(stickShape.getSize().x, stickShape.getSize().y / 2.f);
    stickShape.setPosition(stickPos);
    stickShape.setRotation(angleDeg);

    aimLine[0].position = ballPos + direction * cueBall.getRadius();
    aimLine[0].color = sf::Color(255, 255, 255, 140);
    aimLine[1].position = ballPos + direction * 180.f;
    aimLine[1].color = sf::Color(255, 255, 255, 20);
}

void CueStick::draw(sf::RenderWindow& window) {
    if (isVisible) {
        window.draw(aimLine);
        window.draw(stickShape);
    }
}