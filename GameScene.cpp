#include "GameScene.hpp"
#include <cmath>
#include <string>

namespace {
    void drawText(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
        sf::Vector2f pos, unsigned int size, sf::Color color) {
        sf::Text t(text, font, size);
        t.setPosition(pos);
        t.setFillColor(color);
        window.draw(t);
    }

    std::string groupName(BallGroup g) {
        if (g == BallGroup::Solid) return "Solid";
        if (g == BallGroup::Stripe) return "Stripe";
        return "Belum";
    }
}

GameScene::GameScene(AssetManager& assets)
    : Scene(assets), cue(nullptr),
    powerBar(sf::Vector2f(100.f, 540.f), sf::Vector2f(250.f, 22.f)),
    rack(assets.texture("triangle")),
    players{ Player("Pemain 1"), Player("Pemain 2") },
    currentPlayer(0), state(TurnState::Racking), breakShot(true), charging(false),
    firstHit(nullptr), railHit(false) {
    auto cueBall = std::make_unique<CueBall>(assets.texture("ball_16"), table.headSpot());
    cue = cueBall.get();
    balls.push_back(std::move(cueBall));

    for (int n = 1; n <= 15; ++n) {
        balls.push_back(std::make_unique<ObjectBall>(
            assets.texture("ball_" + std::to_string(n)), table.headSpot(), n));
    }

    if (bgm.openFromFile(PATH_SOUND + "bgm.ogg")) {
        bgm.setLoop(true);
        bgm.setVolume(30.f);
        bgm.play();
    }

    startNewGame();
}

void GameScene::startNewGame() {
    players[0] = Player("Pemain 1");
    players[1] = Player("Pemain 2");
    currentPlayer = 0;
    charging = false;
    powerBar.reset();
    setupRack();
}

void GameScene::setupRack() {
    breakShot = true;
    state = TurnState::Racking;
    message = "Menyusun bola...";

    cue->clearScratch();
    cue->place(table.headSpot());

    std::vector<Ball*> targets;
    for (int n : RackAnimation::makeOrder()) {
        balls[n]->place(table.randomPoint());
        targets.push_back(balls[n].get());
    }
    rack.start(targets, RackAnimation::makeSlots(table.footSpot()));
}

void GameScene::handleEvent(const sf::Event& e, sf::Vector2f mouse) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
        next = SceneID::Menu;
        return;
    }

    bool leftDown = e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left;
    bool leftUp = e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left;
    bool rightDown = e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Right;

    if (state == TurnState::GameOver) {
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::R) startNewGame();
    }
    else if (state == TurnState::PlacingCue) {
        if (leftDown) {
            if (canPlaceCueAt(mouse)) {
                cue->clearScratch();
                state = TurnState::Aiming;
                message.clear();
            }
            else {
                message = "Posisi tidak valid!";
            }
        }
    }
    else if (state == TurnState::Aiming) {
        if (leftDown && !charging) {
            charging = true;
            powerBar.start();
        }
        else if (rightDown && charging) {
            charging = false;
            powerBar.reset();
        }
        else if (leftUp && charging) {
            shoot();
        }
    }
}

void GameScene::update(float dt, sf::Vector2f mouse) {
    if (state == TurnState::Shooting) {
        simulate(dt);
        if (!anyBallMoving()) finishShot();
        return;
    }

    for (auto& b : balls) b->update(dt);

    if (state == TurnState::Racking) {
        rack.update(dt);
        if (rack.isFinished()) {
            state = TurnState::PlacingCue;
            message = "Tempatkan bola putih (klik kiri)";
        }
    }
    else if (state == TurnState::PlacingCue) {
        cue->place(mouse);
    }
    else if (state == TurnState::Aiming) {
        if (charging) powerBar.update(dt);
        float pull = charging ? powerBar.getRatio() * 45.f : 0.f;
        cueStick.update(mouse, *cue, pull);
    }
}

void GameScene::shoot() {
    float power = powerBar.getRatio() * MAX_SHOT_SPEED;
    charging = false;
    powerBar.reset();
    if (power < 20.f) return;

    pocketedThisShot.clear();
    firstHit = nullptr;
    railHit = false;
    cue->setVelocity(cueStick.getDirection() * power);
    assets.playSound("cue_hit");
    state = TurnState::Shooting;
    message.clear();
}

void GameScene::simulate(float dt) {
    float sdt = dt / SUBSTEPS;
    for (int s = 0; s < SUBSTEPS; ++s) {
        for (auto& b : balls) {
            b->update(sdt);
            if (!b->isOnTable()) continue;

            if (table.pocketIndexOf(*b) >= 0) {
                onPocket(*b);
                continue;
            }

            float impact = table.bounceOffCushion(*b);
            if (impact > 0.f) {
                if (impact > 60.f) assets.playSound("wall_hit");
                if (firstHit) railHit = true;
            }
        }

        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                if (!balls[i]->isOnTable() || !balls[j]->isOnTable()) continue;

                float impact = balls[i]->hitBall(*balls[j]);
                if (impact <= 0.f) continue;
                if (impact > 60.f) assets.playSound("ball_hit");

                if (!firstHit) {
                    if (balls[i].get() == cue) firstHit = balls[j].get();
                    else if (balls[j].get() == cue) firstHit = balls[i].get();
                }
            }
        }
    }
}

void GameScene::onPocket(Ball& ball) {
    ball.pocket();
    assets.playSound("pocket");
    if (auto* obj = dynamic_cast<ObjectBall*>(&ball)) {
        pocketedThisShot.push_back(obj);
    }
}

bool GameScene::anyBallMoving() const {
    for (const auto& b : balls) {
        if (b->isMoving()) return true;
    }
    return false;
}

int GameScene::countRemaining(BallGroup group) const {
    int count = 0;
    for (const auto& b : balls) {
        auto* obj = dynamic_cast<const ObjectBall*>(b.get());
        if (obj && obj->isOnTable() && obj->getGroup() == group) ++count;
    }
    return count;
}

bool GameScene::isLegalFirstContact() const {
    auto* hit = dynamic_cast<ObjectBall*>(firstHit);
    if (!hit) return false;

    const Player& shooter = players[currentPlayer];
    bool is8 = hit->getNumber() == 8;

    if (shooter.getGroup() == BallGroup::None) return !is8;
    if (countRemaining(shooter.getGroup()) > 0) return hit->getGroup() == shooter.getGroup();
    return is8;
}

void GameScene::finishShot() {
    Player& shooter = players[currentPlayer];
    Player& opponent = players[1 - currentPlayer];

    bool eightPocketed = false;
    for (ObjectBall* b : pocketedThisShot) {
        if (b->getNumber() == 8) eightPocketed = true;
    }

    if (breakShot && eightPocketed) {
        setupRack();
        message = "Bola 8 masuk saat break, re-rack!";
        return;
    }

    std::string reason;
    bool foul = false;
    if (cue->isScratched()) {
        foul = true;
        reason += "Scratch! ";
    }
    if (!firstHit) {
        foul = true;
        reason += "Tidak menyentuh bola! ";
    }
    else if (!isLegalFirstContact()) {
        foul = true;
        reason += "Bola pertama salah! ";
    }
    if (firstHit && pocketedThisShot.empty() && !railHit) {
        foul = true;
        reason += "Tidak ada pantulan rail! ";
    }
    breakShot = false;

    if (eightPocketed) {
        bool cleared = shooter.getGroup() != BallGroup::None && countRemaining(shooter.getGroup()) == 0;
        endGame((cleared && !foul) ? currentPlayer : 1 - currentPlayer);
        return;
    }

    if (!foul && shooter.getGroup() == BallGroup::None && !pocketedThisShot.empty()) {
        BallGroup g = pocketedThisShot.front()->getGroup();
        shooter.setGroup(g);
        opponent.setGroup(g == BallGroup::Solid ? BallGroup::Stripe : BallGroup::Solid);
    }

    bool legalPocket = false;
    for (ObjectBall* b : pocketedThisShot) {
        for (Player& p : players) {
            if (p.getGroup() == b->getGroup()) p.addScore();
        }
        if (b->getGroup() == shooter.getGroup()) legalPocket = true;
    }

    if (foul) {
        switchTurn();
        state = TurnState::PlacingCue;
        message = reason + "Giliran " + players[currentPlayer].getName() + ": tempatkan bola putih";
    }
    else if (legalPocket) {
        state = TurnState::Aiming;
        message = "Bagus! Tembak lagi";
    }
    else {
        switchTurn();
        state = TurnState::Aiming;
        message = "Giliran " + players[currentPlayer].getName();
    }
}

void GameScene::endGame(int winner) {
    state = TurnState::GameOver;
    message = "Pemenang: " + players[winner].getName();
}

void GameScene::switchTurn() {
    currentPlayer = 1 - currentPlayer;
}

bool GameScene::canPlaceCueAt(sf::Vector2f pos) const {
    if (!table.isInside(pos, BALL_RADIUS)) return false;
    if (breakShot && pos.x > table.headStringX()) return false;

    for (const auto& b : balls) {
        if (b.get() == cue || !b->isOnTable()) continue;
        sf::Vector2f d = b->getPosition() - pos;
        if (std::hypot(d.x, d.y) < 2.f * BALL_RADIUS) return false;
    }
    return true;
}

void GameScene::draw(sf::RenderWindow& window) {
    table.draw(window);
    rack.draw(window);
    for (auto& b : balls) b->draw(window);
    if (state == TurnState::Aiming) cueStick.draw(window);
    powerBar.draw(window);
    drawHUD(window);
}

void GameScene::drawHUD(sf::RenderWindow& window) const {
    const sf::Font& font = assets.font();

    for (int i = 0; i < 2; ++i) {
        std::string info = players[i].getName() + " [" + groupName(players[i].getGroup()) +
            "] : " + std::to_string(players[i].getScore());
        sf::Color color = (i == currentPlayer) ? sf::Color::Yellow : sf::Color::White;
        drawText(window, font, info, sf::Vector2f(20.f + 480.f * i, 12.f), 18, color);
    }

    if (charging) {
        int percent = static_cast<int>(powerBar.getRatio() * 100.f);
        drawText(window, font, "POWER " + std::to_string(percent) + "%",
            sf::Vector2f(100.f, 510.f), 18, sf::Color::White);
    }
    else if (state == TurnState::Aiming) {
        drawText(window, font, "Tahan klik kiri: power | Lepas: pukul | Klik kanan: batal",
            sf::Vector2f(100.f, 510.f), 16, sf::Color(200, 200, 200));
    }

    if (state == TurnState::GameOver) {
        drawText(window, font, message + "   (R = main lagi, ESC = menu)",
            sf::Vector2f(180.f, 260.f), 26, sf::Color::Yellow);
    }
    else {
        drawText(window, font, message, sf::Vector2f(380.f, 540.f), 16, sf::Color::White);
    }
}