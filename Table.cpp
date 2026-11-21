#include "Table.hpp"
#include "GameConfig.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace {
    float bounceAxis(float& pos, float& vel, float minPos, float maxPos) {
        float impact = 0.f;
        if (pos < minPos) {
            pos = minPos;
            if (vel < 0.f) { impact = -vel; vel = -vel * CUSHION_RESTITUTION; }
        }
        else if (pos > maxPos) {
            pos = maxPos;
            if (vel > 0.f) { impact = vel; vel = -vel * CUSHION_RESTITUTION; }
        }
        return impact;
    }
}

Table::Table() : bounds(TABLE_BOUNDS) {
    float l = bounds.left;
    float r = bounds.left + bounds.width;
    float t = bounds.top;
    float b = bounds.top + bounds.height;
    float mx = bounds.left + bounds.width / 2.f;

    pockets = { {l, t}, {mx, t - 4.f}, {r, t}, {l, b}, {mx, b + 4.f}, {r, b} };

    wood.setSize(sf::Vector2f(bounds.width + 2.f * CUSHION, bounds.height + 2.f * CUSHION));
    wood.setPosition(l - CUSHION, t - CUSHION);
    wood.setFillColor(sf::Color(101, 67, 33));

    felt.setSize(sf::Vector2f(bounds.width, bounds.height));
    felt.setPosition(l, t);
    felt.setFillColor(sf::Color(20, 110, 55));

    for (const auto& p : pockets) {
        sf::CircleShape c(POCKET_RADIUS);
        c.setOrigin(POCKET_RADIUS, POCKET_RADIUS);
        c.setPosition(p);
        c.setFillColor(sf::Color(15, 15, 15));
        pocketShapes.push_back(c);
    }
}

void Table::draw(sf::RenderWindow& window) const {
    window.draw(wood);
    window.draw(felt);
    for (const auto& p : pocketShapes) window.draw(p);
}

float Table::bounceOffCushion(Ball& ball) const {
    sf::Vector2f p = ball.getPosition();
    sf::Vector2f v = ball.getVelocity();
    float r = ball.getRadius();

    float ix = bounceAxis(p.x, v.x, bounds.left + r, bounds.left + bounds.width - r);
    float iy = bounceAxis(p.y, v.y, bounds.top + r, bounds.top + bounds.height - r);

    ball.setPosition(p);
    ball.setVelocity(v);
    return std::max(ix, iy);
}

int Table::pocketIndexOf(const Ball& ball) const {
    for (size_t i = 0; i < pockets.size(); ++i) {
        sf::Vector2f d = ball.getPosition() - pockets[i];
        if (std::hypot(d.x, d.y) <= POCKET_RADIUS * 0.9f) return static_cast<int>(i);
    }
    return -1;
}

bool Table::isInside(sf::Vector2f pos, float radius) const {
    return pos.x >= bounds.left + radius && pos.x <= bounds.left + bounds.width - radius &&
        pos.y >= bounds.top + radius && pos.y <= bounds.top + bounds.height - radius;
}

float Table::headStringX() const {
    return bounds.left + bounds.width * 0.25f;
}

sf::Vector2f Table::headSpot() const {
    return sf::Vector2f(headStringX(), bounds.top + bounds.height / 2.f);
}

sf::Vector2f Table::footSpot() const {
    return sf::Vector2f(bounds.left + bounds.width * 0.75f, bounds.top + bounds.height / 2.f);
}

sf::Vector2f Table::randomPoint() const {
    float rx = static_cast<float>(std::rand()) / RAND_MAX;
    float ry = static_cast<float>(std::rand()) / RAND_MAX;
    return sf::Vector2f(bounds.left + 40.f + rx * (bounds.width - 80.f),
        bounds.top + 40.f + ry * (bounds.height - 80.f));
}