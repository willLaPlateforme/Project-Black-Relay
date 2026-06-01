#include <SFML/Graphics.hpp>
#include "Game/Model/Includes/Game.hpp"
#include "Game/Controller/Includes/Game.hpp"
#include "Affichage/Affichage_Menu/Includes/affichage_menu.hpp"
#include "Affichage/Game/Includes/Game.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Tower Defense - Black Relay",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Game           model;
    Controller     controller(window, model);
    Affichage_Menu menuView(window);
    GameView       gameView(window);

    model.addObserver(&gameView);

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        if (model.getState() == GameState::MENU) {
            // Le menu gère sa propre boucle interne
            menuView.handleEvents();
            if (menuView.wantsToQuit()) window.close();
            if (menuView.wantsToPlay()) model.startGame();
        } else {
            controller.handleEvents();
            model.update(dt);
            gameView.render(model);
        }
    }
    return 0;
}
