#pragma once
#include <memory>
#include <string>
#include <vector>
#include <SFML/Audio.hpp>
#include "Ball.hpp"
#include "CueStick.hpp"
#include "Player.hpp"
#include "PowerBar.hpp"
#include "RackAnimation.hpp"
#include "Scene.hpp"
#include "Table.hpp"

enum class TurnState { Racking, PlacingCue, Aiming, Shooting, GameOver };

class GameScene : public Scene {
public:
    explicit GameScene(AssetManager& assets);

    void handleEvent(const sf::Event& e, sf::Vector2f mouse) override;
    void update(float dt, sf::Vector2f mouse) override;
    void draw(sf::RenderWindow& window) override;

private:
    void startNewGame();
    void setupRack();
    void shoot();
    void simulate(float dt);
    void onPocket(Ball& ball);
    void finishShot();
    void endGame(int winner);
    void switchTurn();
    bool canPlaceCueAt(sf::Vector2f pos) const;
    bool isLegalFirstContact() const;
    int countRemaining(BallGroup group) const;
    bool anyBallMoving() const;
    void drawHUD(sf::RenderWindow& window) const;

    Table table;
    std::vector<std::unique_ptr<Ball>> balls;
    CueBall* cue;
    CueStick cueStick;
    PowerBar powerBar;
    RackAnimation rack;
    Player players[2];
    int currentPlayer;
    TurnState state;
    bool breakShot;
    bool charging;
    std::string message;

    std::vector<ObjectBall*> pocketedThisShot;
    Ball* firstHit;
    bool railHit;
    sf::Music bgm;
};