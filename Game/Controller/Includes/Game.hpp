#pragma once
#include <SFML/Graphics.hpp>
#include "../../Model/Includes/Game.hpp"

// ── Controller : traduit les événements utilisateur en actions sur le modèle ──
class Controller {
public:
    Controller(sf::RenderWindow& window, Game& game);

    // Lit et dispatche tous les événements SFML de la frame
    void handleEvents();

private:
    sf::RenderWindow& _window;
    Game&             _game;

    // Gestion du clavier
    void _onKeyPressed(sf::Keyboard::Key key);

    // Clic gauche : placer une tour
    void _onMouseClick(float x, float y);

    // Clic droit : améliorer une tour
    void _onMouseRightClick(float x, float y);
};
