#pragma once
#include "Button.hpp"
#include "Scene.hpp"

class MenuScene : public Scene {
public:
    explicit MenuScene(AssetManager& assets);

    void handleEvent(const sf::Event& e, sf::Vector2f mouse) override;
    void update(float, sf::Vector2f mouse) override;
    void draw(sf::RenderWindow& window) override;

private:
    sf::Text title;
    Button startButton;
    Button exitButton;
};