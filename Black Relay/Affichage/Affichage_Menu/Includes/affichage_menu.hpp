#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include "../../../Menu/Includes/Menu.hpp"
#include "../../../Menu/Includes/Options.hpp"

enum class MenuState { MAIN, OPTIONS, SCORES };

// Classe Affichage_Menu : gere tout l'affichage du menu principal
// Recoit les donnees de Menu (logique) et les dessine dans la fenetre
class Affichage_Menu {
public:
    explicit Affichage_Menu(sf::RenderWindow& window);

    // Boucle principale : gere les evenements et appelle le rendu
    void handleEvents();

    // Dessine le fond uniquement (utilise dans la boucle principale)
    void render();

    bool wantsToPlay()       const;
    bool wantsToQuit()       const;
    bool isSoundOn()         const;

    // Retourne la difficulte choisie dans les options
    Difficulte getDifficulte() const;

private:
    sf::RenderWindow& _window;
    sf::Font          _font;

    // Texture et sprite du fond
    sf::Texture               _backgroundTexture;
    std::optional<sf::Sprite> _backgroundSprite;

    // Formes geometriques pour les panels
    sf::RectangleShape _leftPanel;
    sf::RectangleShape _selectBar;
    sf::RectangleShape _newsPanel;

    MenuState  _menuState;
    bool       _play;
    bool       _quit;
    bool       _soundEnabled;

    // Difficulte choisie dans les options
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