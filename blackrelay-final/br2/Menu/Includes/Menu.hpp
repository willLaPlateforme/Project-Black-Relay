#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

struct MenuItem { std::string label; };

class Menu {
public:
    explicit Menu(sf::RenderWindow& window);
    void handleEvents();
    void render();
    int  getActivated() const;
    void resetActivated();

    static const int CONTINUER       = 0;
    static const int NOUVELLE_PARTIE = 1;
    static const int PARAMETRES      = 2;
    static const int CREDITS         = 3;
    static const int QUITTER         = 4;

private:
    sf::RenderWindow& _window;
    sf::Font          _font;
    sf::Texture       _bgTexture;
    bool              _hasBg = false;

    std::vector<MenuItem> _items;
    int   _selectedIndex;
    int   _activatedIndex;
    float _animTimer;

    sf::RectangleShape _leftPanel;
    sf::RectangleShape _selectBar;
    sf::RectangleShape _newsPanel;

    void _drawBackground();
    void _drawMenuItems();
    void _drawStatusPanel();
    void _drawNewsPanel();
    void _drawScanlines();
    void _drawCornerDeco();
};

class Options {
public:
    Options(sf::RenderWindow& window, sf::Font& font);
    void handleEvents(bool& back);
    void render();
    void toggleSound();
    bool isSoundEnabled() const;
private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    bool              _soundEnabled;
    sf::Texture       _bgTexture;
    bool              _hasBg = false;
};

class Scores {
public:
    Scores(sf::RenderWindow& window, sf::Font& font);
    void addScore(const std::string& name, int score);
    void handleEvents(bool& back);
    void render();
private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    struct Entry { std::string name; int score; };
    std::vector<Entry> _entries;
    sf::Texture        _bgTexture;
    bool               _hasBg = false;
};
