#include <SFML/Graphics.hpp>
#include "Affichage/Affichage_Menu/Includes/affichage_menu.hpp"
#include "Game/Model/Includes/Game.hpp"
#include "Game/Controller/Includes/Game.hpp"
#include "Affichage/Game/Includes/Game.hpp"

int main() {
    sf::RenderWindow window(
        sf::VideoMode({1280, 720}),
        "Black Relay",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("Assets/Icones/Ico/Black Relay.png"))
        window.setIcon(icon);

    Game           model;
    Affichage_Menu menuView(window);

    while (window.isOpen()) {

        menuView.handleEvents();

        if (menuView.wantsToQuit()) {
            window.close();
            break;
        }

        if (menuView.wantsToPlay()) {
            // ── Transition → jeu 1448x1030 ───────────────────────────────────
            window.close();
            window.create(
                sf::VideoMode({1448, 1030}),
                "Black Relay - In Game",
                sf::Style::Titlebar | sf::Style::Close
            );
            window.setFramerateLimit(60);
            if (icon.getSize().x > 0) window.setIcon(icon);

            Controller controller(window, model);
            GameView   gameView(window);
            model.addObserver(&gameView);

            model.setSoundEnabled(menuView.isSoundOn());
            model.setDifficulte(menuView.getDifficulte());
            model.startGame();

            sf::Clock clock;
            bool backToMenu    = false;
            bool scoreSaved    = false; // éviter de sauvegarder plusieurs fois

            while (window.isOpen() && !backToMenu) {
                float dt = clock.restart().asSeconds();

                controller.handleEvents();

                // ESC → retour menu
                if (model.getState() == GameState::MENU) {
                    backToMenu = true;
                    break;
                }

                model.update(dt);
                gameView.render(model);

                // ── Fin de partie : sauvegarder le score une seule fois ───────
                if (!scoreSaved &&
                    (model.getState() == GameState::VICTORY ||
                     model.getState() == GameState::DEFEAT)) {
                    if (model.getScore() > 0)
                        menuView.saveScore("CMD", model.getScore());
                    scoreSaved = true;
                }

                // ── Le joueur a appuyé ENTREE → retour menu ───────────────────
                if (scoreSaved && model.getState() == GameState::MENU)
                    backToMenu = true;
            }

            if (backToMenu && window.isOpen()) {
                // ── Transition → menu 1280x720 ────────────────────────────────
                window.close();
                window.create(
                    sf::VideoMode({1280, 720}),
                    "Black Relay",
                    sf::Style::Titlebar | sf::Style::Close
                );
                window.setFramerateLimit(60);
                if (icon.getSize().x > 0) window.setIcon(icon);
                menuView.rebind(window);
            }
        }
    }

    return 0;
}