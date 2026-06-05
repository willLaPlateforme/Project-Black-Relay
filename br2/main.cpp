#include <SFML/Graphics.hpp>
#include "Game/Model/Includes/Game.hpp"
#include "Game/Controller/Includes/Game.hpp"
#include "Affichage/Affichage_Menu/Includes/affichage_menu.hpp"
#include "Affichage/Game/Includes/Game.hpp"

int main() {
    // ── Fenêtre principale ────────────────────────────────────────────────────
    sf::RenderWindow window(
    sf::VideoMode({1448, 1030}),
    "Tower Defense - Black Relay",
    sf::Style::Titlebar | sf::Style::Close
);

    window.setFramerateLimit(60);

    // ── Création des composants MVC ───────────────────────────────────────────
    Game           model;                    // Modèle : logique du jeu
    Controller     controller(window, model); // Contrôleur : événements
    Affichage_Menu menuView(window);          // Vue : menu principal
    GameView       gameView(window);          // Vue : jeu en cours

    // Branchement Observer : la vue reçoit les notifications du modèle
    model.addObserver(&gameView);

    // ── Boucle principale du jeu ──────────────────────────────────────────────
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        if (model.getState() == GameState::MENU) {
            // ── Boucle menu ───────────────────────────────────────────────────
            menuView.handleEvents();

            if (menuView.wantsToQuit())
                window.close();

            if (menuView.wantsToPlay()) {
                // Transmettre le réglage son du menu vers le modèle
                model.setSoundEnabled(menuView.isSoundOn());
                model.startGame();
            }

        } else {
            // ── Boucle jeu ────────────────────────────────────────────────────
            controller.handleEvents();
            model.update(dt);
            gameView.render(model);

            // Sauvegarder le score en fin de partie
            if (model.getState() == GameState::VICTORY ||
                model.getState() == GameState::DEFEAT) {
                if (model.getScore() > 0) {
                    menuView.saveScore("CMD", model.getScore());
                }
            }
        }
    }

    return 0;
}
