#include "RackAnimation.hpp"
#include "GameConfig.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace {
    const float MOVE_TIME = 0.8f;
    const float STAGGER = 0.05f;
}

RackAnimation::RackAnimation(const sf::Texture& triangleTex)
    : triangle(triangleTex), elapsed(0.f), total(1.f), finished(true) {}

void RackAnimation::start(const std::vector<Ball*>& balls, const std::vector<sf::Vector2f>& slots) {
    moves.clear();
    elapsed = 0.f;
    finished = false;

    for (size_t i = 0; i < balls.size(); ++i) {
        moves.push_back({ balls[i], balls[i]->getPosition(), slots[i], i * STAGGER });
    }
    total = (balls.size() - 1) * STAGGER + MOVE_TIME;

    float minX = slots[0].x, maxX = slots[0].x, minY = slots[0].y, maxY = slots[0].y;
    for (const auto& s : slots) {
        minX = std::min(minX, s.x);
        maxX = std::max(maxX, s.x);
        minY = std::min(minY, s.y);
        maxY = std::max(maxY, s.y);
    }

    sf::Vector2u size = triangle.getTexture()->getSize();
    float w = maxX - minX + 4.f * BALL_RADIUS;
    float h = maxY - minY + 4.f * BALL_RADIUS;
    triangle.setOrigin(size.x / 2.f, size.y / 2.f);
    triangle.setPosition((minX + maxX) / 2.f, (minY + maxY) / 2.f);
    triangle.setScale(w / size.x, h / size.y);
}

void RackAnimation::update(float dt) {
    if (finished) return;
    elapsed += dt;

    bool allDone = true;
    for (auto& m : moves) {
        float t = std::clamp((elapsed - m.delay) / MOVE_TIME, 0.f, 1.f);
        float e = t * t * (3.f - 2.f * t);
        m.ball->setPosition(m.from + (m.to - m.from) * e);
        if (t < 1.f) allDone = false;
    }
    if (allDone) finished = true;
}

void RackAnimation::draw(sf::RenderWindow& window) {
    if (finished) return;
    float fade = 1.f - std::min(elapsed / total, 1.f);
    triangle.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(160 * fade)));
    window.draw(triangle);
}

bool RackAnimation::isFinished() const { return finished; }

std::vector<int> RackAnimation::makeOrder() {
    static std::mt19937 rng(std::random_device{}());
    std::vector<int> nums;
    do {
        nums.clear();
        for (int n = 1; n <= 15; ++n) {
            if (n != 8) nums.push_back(n);
        }
        std::shuffle(nums.begin(), nums.end(), rng);
        nums.insert(nums.begin() + 4, 8);
    } while ((nums[10] < 8) == (nums[14] < 8));
    return nums;
}

std::vector<sf::Vector2f> RackAnimation::makeSlots(sf::Vector2f apex) {
    std::vector<sf::Vector2f> slots;
    float dx = std::sqrt(3.f) * BALL_RADIUS + 0.5f;
    float dy = 2.f * BALL_RADIUS + 0.5f;
    for (int row = 0; row < 5; ++row) {
        for (int i = 0; i <= row; ++i) {
            slots.push_back(sf::Vector2f(apex.x + row * dx, apex.y + (i - row / 2.f) * dy));
        }
    }
    return slots;
}