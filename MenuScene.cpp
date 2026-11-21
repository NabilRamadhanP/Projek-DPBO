#include "MenuScene.hpp"
#include "GameConfig.hpp"

MenuScene::MenuScene(AssetManager& assets)
    : Scene(assets),
    title("BILLIARD", assets.font(), 64),
    startButton(assets.font(), "START", sf::Vector2f(400.f, 250.f), sf::Vector2f(200.f, 60.f)),
    exitButton(assets.font(), "EXIT", sf::Vector2f(400.f, 340.f), sf::Vector2f(200.f, 60.f)) {
    sf::FloatRect b = title.getLocalBounds();
    title.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    title.setPosition(WINDOW_WIDTH / 2.f, 150.f);
    title.setFillColor(sf::Color::Yellow);
}

void MenuScene::handleEvent(const sf::Event& e, sf::Vector2f mouse) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (startButton.contains(mouse)) next = SceneID::Game;
        else if (exitButton.contains(mouse)) next = SceneID::Exit;
    }
}

void MenuScene::update(float, sf::Vector2f mouse) {
    startButton.update(mouse);
    exitButton.update(mouse);
}

void MenuScene::draw(sf::RenderWindow& window) {
    window.draw(title);
    startButton.draw(window);
    exitButton.draw(window);
}