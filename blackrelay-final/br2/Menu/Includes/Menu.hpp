#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

// ── MenuItem : entrée du menu principal ──────────────────────────────────────
struct MenuItem { std::string label; };

// ── Menu : écran principal avec navigation clavier ────────────────────────────
class Menu {
public:
    explicit Menu(sf::RenderWindow& window);
    void handleEvents();
    void render();
    int  getActivated() const;
    void resetActivated();

    // Indices des entrées du menu principal
    static const int CONTINUER       = 0;
    static const int NOUVELLE_PARTIE = 1;
    static const int SCORES          = 2;
    static const int PARAMETRES      = 3;
    static const int CREDITS         = 4;
    static const int QUITTER         = 5;

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

// ── Options : écran des paramètres (son on/off) ────────────────────────────────
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

// ── Scores : tableau des meilleurs scores avec persistance fichier ─────────────
class Scores {
public:
    Scores(sf::RenderWindow& window, sf::Font& font);

    // Ajoute un score et sauvegarde dans Assets/Data/scores.txt
    void addScore(const std::string& name, int score);

    void handleEvents(bool& back);
    void render();

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;

    struct Entry { std::string name; int score; };
    std::vector<Entry> _entries;

    sf::Texture _bgTexture;
    bool        _hasBg = false;

    void _saveToFile();   // sauvegarde dans scores.txt
    void _loadFromFile(); // charge depuis scores.txt
};
