#pragma once
#include <string>
#include "GameConfig.hpp"

class Player {
public:
    Player(const std::string& name) : name_(name), group_(BallGroup::None), score_(0) {}

    const std::string& getName() const { return name_; }
    BallGroup getGroup() const { return group_; }
    void setGroup(BallGroup g) { group_ = g; }
    int getScore() const { return score_; }
    void addScore() { ++score_; }

private:
    std::string name_;
    BallGroup group_;
    int score_;
};