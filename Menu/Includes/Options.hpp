#pragma once
#include <SFML/Graphics.hpp>

enum class Difficulte { FACILE, NORMAL, DIFFICILE, ENDLESS };

class Menu;

class Options {
public:
    Options(sf::RenderWindow& window, sf::Font& font, Menu& menu);
    void handleEvents(bool& back);
    bool isSoundEnabled() const;
    Difficulte getDifficulte() const;

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Menu&             _menu;

    bool        _soundEnabled = true;
    Difficulte  _difficulte   = Difficulte::NORMAL;

    sf::FloatRect _soundBtnRect  = {{400.f, 250.f}, {240.f, 45.f}};
    sf::FloatRect _facileRect    = {{340.f, 370.f}, {120.f, 40.f}};
    sf::FloatRect _normalRect    = {{470.f, 370.f}, {120.f, 40.f}};
    sf::FloatRect _difficileRect = {{600.f, 370.f}, {120.f, 40.f}};
    sf::FloatRect _endlessRect   = {{730.f, 370.f}, {120.f, 40.f}};
    sf::FloatRect _backBtnRect   = {{490.f, 500.f}, {240.f, 45.f}};

    sf::Vector2f _mousePos;
};
