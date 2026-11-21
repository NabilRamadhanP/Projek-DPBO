#include "AssetManager.hpp"
#include <iostream>
#include "GameConfig.hpp"

AssetManager::AssetManager() : soundPool(12) {
    for (int i = 1; i <= 16; ++i) {
        loadTexture("ball_" + std::to_string(i), PATH_BALL + std::to_string(i) + ".png");
    }
    loadTexture("triangle", PATH_TRIANGLE);

    loadSound("cue_hit", PATH_SOUND + "cue_hit.wav");
    loadSound("wall_hit", PATH_SOUND + "wall_hit.wav");
    loadSound("ball_hit", PATH_SOUND + "ball_hit.wav");
    loadSound("pocket", PATH_SOUND + "pocket.wav");

    if (!mainFont.loadFromFile(PATH_FONT)) {
        std::cerr << "Font gagal dimuat: " << PATH_FONT << "\n";
    }
}

const sf::Texture& AssetManager::texture(const std::string& name) const {
    return textures.at(name);
}

const sf::Font& AssetManager::font() const {
    return mainFont;
}

void AssetManager::playSound(const std::string& name) {
    sf::Sound* sound = &soundPool[0];
    for (auto& s : soundPool) {
        if (s.getStatus() == sf::SoundSource::Stopped) {
            sound = &s;
            break;
        }
    }
    sound->setBuffer(buffers.at(name));
    sound->play();
}

void AssetManager::loadTexture(const std::string& name, const std::string& path) {
    if (!textures[name].loadFromFile(path)) {
        std::cerr << "Gagal load gambar: " << path << "\n";
    }
    textures[name].setSmooth(true);
}

void AssetManager::loadSound(const std::string& name, const std::string& path) {
    if (!buffers[name].loadFromFile(path)) {
        std::cerr << "Gagal load suara: " << path << "\n";
    }
}