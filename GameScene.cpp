#include "GameScene.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace {
    const sf::Color ACTIVE_COLOR(255, 225, 60);
    const sf::Color INACTIVE_COLOR(150, 150, 150);

    void drawText(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
        sf::Vector2f pos, unsigned int size, sf::Color color) {
        sf::Text t(text, font, size);
        t.setPosition(pos);
        t.setFillColor(color);
        window.draw(t);
    }

    void drawCentered(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
        float cx, float cy, unsigned int size, sf::Color color) {
        sf::Text t(text, font, size);
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(cx, cy);
        t.setFillColor(color);
        window.draw(t);
    }

    float panelCenter(bool right) {
        if (right) return (WINDOW_WIDTH + TABLE_BOUNDS.left + TABLE_BOUNDS.width + CUSHION) / 2.f;
        return (TABLE_BOUNDS.left - CUSHION) / 2.f;
    }

    std::string groupName(BallGroup g) {
        if (g == BallGroup::Solid) return "Solid";
        if (g == BallGroup::Stripe) return "Stripe";
        return "Belum";
    }
}

GameScene::GameScene(AssetManager& assets)
    : Scene(assets), cue(nullptr),
    powerBar(sf::Vector2f(200.f, 562.f), sf::Vector2f(250.f, 22.f)),
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
        if (auto* hit = dynamic_cast<ObjectBall*>(firstHit)) {
            for (ObjectBall* b : pocketedThisShot) {
                if (b->getGroup() == hit->getGroup()) g = hit->getGroup();
            }
        }
        shooter.setGroup(g);
        opponent.setGroup(g == BallGroup::Solid ? BallGroup::Stripe : BallGroup::Solid);
    }

    bool legalPocket = false;
    for (ObjectBall* b : pocketedThisShot) {
        for (Player& p : players) {
            if (p.getGroup() == b->getGroup()) p.addBall(b->getNumber());
        }
        if (!foul && b->getGroup() == shooter.getGroup()) legalPocket = true;
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
    if (breakShot && pos.x > table.headStringX()) return false;
    return isSpotFree(pos, cue);
}

bool GameScene::isSpotFree(sf::Vector2f pos, const Ball* ignore) const {
    if (!table.isInside(pos, BALL_RADIUS)) return false;

    for (const auto& b : balls) {
        if (b.get() == ignore || !b->isOnTable()) continue;
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
    const sf::Font& bold = assets.boldFont();

    for (int i = 0; i < 2; ++i) {
        const Player& p = players[i];
        bool active = (i == currentPlayer);
        float cx = panelCenter(i == 1);
        sf::Color accent = active ? ACTIVE_COLOR : INACTIVE_COLOR;

        drawCentered(window, bold, "SCORE " + std::to_string(p.getScore()), cx, 120.f, 32, accent);

        sf::RectangleShape line(sf::Vector2f(130.f, 3.f));
        line.setOrigin(65.f, 1.5f);
        line.setPosition(cx, 180.f);
        line.setFillColor(accent);
        window.draw(line);

        const std::vector<int>& nums = p.getBalls();
        int count = static_cast<int>(nums.size());
        for (int k = 0; k < count; ++k) {
            int row = k / 4;
            int col = k % 4;
            int inRow = std::min(4, count - row * 4);
            float x = cx + (col - (inRow - 1) / 2.f) * 30.f;
            float y = 217.f + row * 30.f;

            const sf::Texture& tex = assets.texture("ball_" + std::to_string(nums[k]));
            sf::Sprite s(tex);
            float scale = 24.f / tex.getSize().x;
            s.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
            s.setPosition(x, y);
            s.setScale(scale, scale);
            window.draw(s);
        }

        drawCentered(window, bold, p.getName(), cx, 290.f, 24, accent);
        drawCentered(window, font, "[" + groupName(p.getGroup()) + "]", cx, 322.f, 16, accent);

        const sf::Texture& avTex = assets.texture(i == 0 ? "player_1" : "player_2");
        sf::Sprite avatar(avTex);
        float avScale = 120.f / avTex.getSize().x;
        avatar.setOrigin(avTex.getSize().x / 2.f, avTex.getSize().y / 2.f);
        avatar.setPosition(cx, 430.f);
        avatar.setScale(avScale, avScale);
        avatar.setColor(active ? sf::Color::White : sf::Color(110, 110, 110));
        window.draw(avatar);
    }

    if (charging) {
        int percent = static_cast<int>(powerBar.getRatio() * 100.f);
        drawText(window, font, "POWER " + std::to_string(percent) + "%",
            sf::Vector2f(200.f, 532.f), 18, sf::Color::White);
    }
    else if (state == TurnState::Aiming) {
        drawText(window, font, "Tahan klik kiri: power | Lepas: pukul | Klik kanan: batal",
            sf::Vector2f(200.f, 532.f), 16, sf::Color(200, 200, 200));
    }

    if (state == TurnState::GameOver) {
        drawCentered(window, bold, message, WINDOW_WIDTH / 2.f, 250.f, 28, ACTIVE_COLOR);
        drawCentered(window, font, "(R = main lagi, ESC = menu)", WINDOW_WIDTH / 2.f, 290.f, 16, INACTIVE_COLOR);
    }
    else {
        drawText(window, font, message, sf::Vector2f(470.f, 560.f), 16, sf::Color::White);
    }
}