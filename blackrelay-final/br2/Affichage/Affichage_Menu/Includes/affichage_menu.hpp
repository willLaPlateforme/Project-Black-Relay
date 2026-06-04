#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class MenuState { MAIN, OPTIONS, SCORES, CREDITS };

// ── Affichage_Menu : orchestre tous les écrans du menu ────────────────────────
// Gère la navigation entre Menu, Options, Scores et Crédits
class Affichage_Menu {
public:
    explicit Affichage_Menu(sf::RenderWindow& window);

    // Boucle de gestion du menu (bloquante jusqu'à une action)
    void handleEvents();
    void render();

    // Sauvegarde un score en fin de partie
    void saveScore(const std::string& name, int score);

    bool wantsToPlay() const;
    bool wantsToQuit() const;
    bool isSoundOn()   const;

private:
    sf::RenderWindow& _window;
    sf::Font          _font;
    MenuState         _menuState;
    bool              _play, _quit, _soundEnabled;
};
