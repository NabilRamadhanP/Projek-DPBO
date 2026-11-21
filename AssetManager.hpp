#pragma once
#include <map>
#include <string>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class AssetManager {
public:
    AssetManager();

    const sf::Texture& texture(const std::string& name) const;
    const sf::Font& font() const;
    const sf::Font& boldFont() const;
    void playSound(const std::string& name);

private:
    void loadTexture(const std::string& name, const std::string& path);
    void loadSound(const std::string& name, const std::string& path);

    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::SoundBuffer> buffers;
    std::vector<sf::Sound> soundPool;
    sf::Font mainFont;
    sf::Font strongFont;
};