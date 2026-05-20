#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <iostream>

// Central asset manager — all textures, font, sounds loaded once.
// Returns nullptr / silence gracefully if a file is missing.

class Assets {
public:
    static Assets& get() {
        static Assets instance;
        return instance;
    }

    // Accepts "p1_idle" or "p1_idle.png"
    sf::Texture* texture(const std::string& name) {
        auto it = textures.find(name);
        if (it != textures.end()) return &it->second;

        std::string path = "assets/" + name;
        if (path.find('.') == std::string::npos) path += ".png";

        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            textures[name] = std::move(tex);
            return &textures[name];
        }
        std::cout << "[Assets] Missing: " << path << "\n";
        return nullptr;
    }

    // Accepts font() or font("main")
    sf::Font* font(const std::string& = "main") {
        if (fontLoaded) return &mainFont;
        if (mainFont.loadFromFile("assets/font.ttf")) {
            fontLoaded = true;
            return &mainFont;
        }
        std::cout << "[Assets] Missing: assets/font.ttf\n";
        return nullptr;
    }

    // Accepts "gunshot" or "gunshot.wav"
    sf::SoundBuffer* sound(const std::string& name) {
        auto it = soundBuffers.find(name);
        if (it != soundBuffers.end()) return &it->second;

        std::string path = "assets/" + name;
        if (path.find('.') == std::string::npos) path += ".wav";

        sf::SoundBuffer buf;
        if (buf.loadFromFile(path)) {
            soundBuffers[name] = std::move(buf);
            return &soundBuffers[name];
        }
        std::cout << "[Assets] Missing: " << path << "\n";
        return nullptr;
    }

    void loadAll() {
        // Textures
        texture("background");   texture("platform");     texture("ladder");
        texture("p1_idle");      texture("p2_idle");
        texture("p1_run");       texture("p2_run");
        texture("p1_jump");      texture("p2_jump");
        texture("p1_windup");    texture("p2_windup");
        texture("p1_reload");    texture("p2_reload");
        texture("p1_victory");   texture("p2_victory");
        texture("ladder_climb_a"); texture("ladder_climb_b");
        texture("gun");          texture("logo");
        texture("menu_bg");      texture("ammo_full");    texture("ammo_empty");
        // Sounds
        sound("gunshot");  sound("reload");    sound("round_win");
        sound("menu_music"); sound("fight_music");
        // Font
        font();
    }

private:
    Assets() = default;
    std::map<std::string, sf::Texture>     textures;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    sf::Font mainFont;
    bool     fontLoaded = false;
};
