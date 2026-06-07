#pragma once
#include <SFML/Graphics.hpp>
#include "../../Model/Includes/Game.hpp"

// Controller : traduit les événements utilisateur en actions sur le modèle
class Controller {
public:
    Controller(sf::RenderWindow& window, Game& game);
    void handleEvents();

private:
    sf::RenderWindow& _window;
    Game&             _game;

    void _onKeyPressed(sf::Keyboard::Key key);
    void _onMouseClick(float x, float y);
    void _onMouseRightClick(float x, float y);
};
