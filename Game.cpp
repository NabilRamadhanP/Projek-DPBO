#include "Game.hpp"
#include <algorithm>
#include "GameScene.hpp"
#include "MenuScene.hpp"
#include "GameConfig.hpp"

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Simulasi Permainan Billiard - Kelompok 6") {
    window.setFramerateLimit(60);
    changeScene(SceneID::Menu);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            scene->handleEvent(event, mouse);
        }

        scene->update(dt, mouse);

        SceneID next = scene->getNext();
        if (next == SceneID::Exit) window.close();
        else if (next != SceneID::None) changeScene(next);

        if (!window.isOpen()) break;

        window.clear(sf::Color(30, 32, 36));
        scene->draw(window);
        window.display();
    }
}

void Game::changeScene(SceneID id) {
    if (id == SceneID::Game) scene = std::make_unique<GameScene>(assets);
    else scene = std::make_unique<MenuScene>(assets);
}