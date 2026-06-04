#pragma once
#include <SFML/Graphics.hpp>

class Options {
public:
    Options(sf::RenderWindow& window, sf::Font& font);
    void handleEvents(bool& back);
    void render();
    bool isSoundEnabled() const;

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    bool              _soundEnabled = true;
    sf::Vector2f      _mousePos;

    sf::FloatRect _soundBtnRect = {{300.f, 250.f}, {200.f, 40.f}};
    sf::FloatRect _backBtnRect  = {{300.f, 400.f}, {200.f, 40.f}};
};