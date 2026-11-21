#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "AssetManager.hpp"
#include "Scene.hpp"

class Game {
public:
    Game();
    void run();

private:
    void changeScene(SceneID id);

    sf::RenderWindow window;
    AssetManager assets;
    std::unique_ptr<Scene> scene;
};