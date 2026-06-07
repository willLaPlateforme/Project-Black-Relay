#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <string>
#include "../../../Menu/Includes/Menu.hpp"
#include "../../../Menu/Includes/Options.hpp"
#include "../../../Menu/Includes/Scores.hpp"

enum class MenuState { MAIN, OPTIONS, SCORES };

// Classe Affichage_Menu : gère tout l'affichage du menu principal
// Reçoit les données de Menu (logique) et les dessine dans la fenêtre
class Affichage_Menu {
public:
    explicit Affichage_Menu(sf::RenderWindow& window);

    // Boucle interne menu : gère événements + rendu jusqu'à play/quit
    void handleEvents();

    // Rendu minimaliste (fond seul, utilisé si besoin externe)
    void render();

    // Sauvegarde un score (appelé depuis main après une partie)
    void saveScore(const std::string& name, int score);

    // Rebind vers une nouvelle fenêtre (après redimensionnement)
    void rebind(sf::RenderWindow& window);

    bool wantsToPlay()       const;
    bool wantsToQuit()       const;
    bool isSoundOn()         const;

    // Difficulté choisie dans les options
    Difficulte getDifficulte() const;

private:
    sf::RenderWindow* _window;   // pointeur pour permettre rebind
    sf::Font          _font;

    // Texture et sprite du fond
    sf::Texture               _backgroundTexture;
    std::optional<sf::Sprite> _backgroundSprite;

    // Formes géométriques pour les panels
    sf::RectangleShape _leftPanel;
    sf::RectangleShape _selectBar;
    sf::RectangleShape _newsPanel;

    // Scores accumulés
    Scores _scores;

    MenuState  _menuState;
    bool       _play;
    bool       _quit;
    bool       _soundEnabled;

    // Difficulté choisie
    Difficulte _difficulte = Difficulte::NORMAL;

    float        _animTimer;
    sf::Vector2f _mousePos;

    // Fonctions d'affichage
    void _drawBackground();
    void _drawMenuItems(const Menu& menu);
    void _drawStatusPanel();
    void _drawNewsPanel();
    void _drawScanlines();
    void _drawCornerDeco();
};
