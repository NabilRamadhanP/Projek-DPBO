#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManager.hpp"

enum class SceneID { None, Menu, Game, Exit };

class Scene {
public:
    explicit Scene(AssetManager& assets) : assets(assets), next(SceneID::None) {}
    virtual ~Scene() = default;

    virtual void handleEvent(const sf::Event& e, sf::Vector2f mouse) = 0;
    virtual void update(float dt, sf::Vector2f mouse) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    SceneID getNext() const { return next; }

protected:
    AssetManager& assets;
    SceneID next;
};