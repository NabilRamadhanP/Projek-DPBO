#pragma once
#include <string>
#include <vector>
#include "GameConfig.hpp"

class Player {
public:
    Player(const std::string& name) : name_(name), group_(BallGroup::None) {}

    const std::string& getName() const { return name_; }
    BallGroup getGroup() const { return group_; }
    void setGroup(BallGroup g) { group_ = g; }
    int getScore() const { return static_cast<int>(balls_.size()); }
    const std::vector<int>& getBalls() const { return balls_; }
    void addBall(int number) { balls_.push_back(number); }

private:
    std::string name_;
    BallGroup group_;
    std::vector<int> balls_;
};