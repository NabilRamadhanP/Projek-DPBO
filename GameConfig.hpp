#pragma once
#include <string>
#include <SFML/Graphics.hpp>

const unsigned int WINDOW_WIDTH = 1200;
const unsigned int WINDOW_HEIGHT = 600;

const sf::FloatRect TABLE_BOUNDS(200.f, 80.f, 800.f, 420.f);
const float CUSHION = 25.f;
const float BALL_RADIUS = 13.f;
const float POCKET_RADIUS = 22.f;

const float FRICTION = 0.985f;
const float BALL_RESTITUTION = 0.95f;
const float CUSHION_RESTITUTION = 0.85f;
const float MAX_SHOT_SPEED = 1300.f;
const float POWER_SPEED = 1.0f;
const int SUBSTEPS = 8;
const float PI = 3.14159265f;

enum class BallGroup { None, Solid, Stripe };

const std::string PATH_BALL = "assets/images/ball_";
const std::string PATH_TRIANGLE = "assets/images/triangle.png";
const std::string PATH_PLAYER = "assets/images/player_";
const std::string PATH_SOUND = "assets/sounds/";
const std::string PATH_FONT = "assets/fonts/PixelifySans-Regular.ttf";
const std::string PATH_FONT_BOLD = "assets/fonts/PixelifySans-Bold.ttf";