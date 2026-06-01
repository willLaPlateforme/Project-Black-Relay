#pragma once
#include <SFML/Graphics.hpp>

enum class MenuState { MAIN, OPTIONS, SCORES };

// ── Affichage_Menu : orchestre Menu / Options / Scores ────────────────────────

class Affichage_Menu {
public:
    explicit Affichage_Menu(sf::RenderWindow& window);

    void handleEvents();
    void render();

    bool wantsToPlay() const;
    bool wantsToQuit() const;
    bool isSoundOn()   const;

private:
    sf::RenderWindow& _window;
    sf::Font          _font;
    MenuState         _menuState;
    bool              _play, _quit, _soundEnabled;
};
