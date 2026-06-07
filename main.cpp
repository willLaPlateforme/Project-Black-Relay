#include <SFML/Graphics.hpp>
#include "Affichage/Affichage_Menu/Includes/affichage_menu.hpp"
#include "Game/Model/Includes/Game.hpp"
#include "Game/Controller/Includes/Game.hpp"
#include "Affichage/Game/Includes/Game.hpp"

int main() {
    // ── Phase Menu : fenêtre 1280x720 (résolution collègue) ──────────────────
    sf::RenderWindow window(
        sf::VideoMode({1280, 720}),
        "Black Relay",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    // Icône
    sf::Image icon;
    if (icon.loadFromFile("Assets/Icones/Ico/Black Relay.png"))
        window.setIcon(icon);

    // ── Composants MVC ────────────────────────────────────────────────────────
    Game           model;
    Affichage_Menu menuView(window);

    // ── Boucle principale ──────────────────────────────────────────────────────
    while (window.isOpen()) {

        // ── ÉTAT MENU ─────────────────────────────────────────────────────────
        // handleEvents() de Affichage_Menu gère sa propre boucle interne
        menuView.handleEvents();

        if (menuView.wantsToQuit()) {
            window.close();
            break;
        }

        if (menuView.wantsToPlay()) {
            // ── Transition : redimensionner en 1448x1030 pour le jeu ─────────
            window.close();
            window.create(
                sf::VideoMode({1448, 1030}),
                "Black Relay - In Game",
                sf::Style::Titlebar | sf::Style::Close
            );
            window.setFramerateLimit(60);
            if (icon.getSize().x > 0) window.setIcon(icon);

            // Instancier Controller et GameView maintenant que la fenêtre est en 1448x1030
            Controller controller(window, model);
            GameView   gameView(window);
            model.addObserver(&gameView);

            // Transmettre les réglages du menu
            model.setSoundEnabled(menuView.isSoundOn());
            model.setDifficulte(menuView.getDifficulte());
            model.startGame();

            // ── BOUCLE JEU ────────────────────────────────────────────────────
            sf::Clock clock;
            bool backToMenu = false;

            while (window.isOpen() && !backToMenu) {
                float dt = clock.restart().asSeconds();

                controller.handleEvents();

                // ESC depuis le jeu → retour menu
                if (model.getState() == GameState::MENU) {
                    backToMenu = true;
                    break;
                }

                model.update(dt);
                gameView.render(model);

                // Fin de partie : sauvegarder le score
                if (model.getState() == GameState::VICTORY ||
                    model.getState() == GameState::DEFEAT) {
                    if (model.getScore() > 0)
                        menuView.saveScore("CMD", model.getScore());
                    // Attendre que le joueur appuie sur ENTREE (géré dans gameView/controller)
                    // Quand il le fait, le modèle repasse en MENU
                    if (model.getState() == GameState::MENU)
                        backToMenu = true;
                }
            }

            if (backToMenu && window.isOpen()) {
                // ── Retour : redimensionner en 1280x720 pour le menu ─────────
                window.close();
                window.create(
                    sf::VideoMode({1280, 720}),
                    "Black Relay",
                    sf::Style::Titlebar | sf::Style::Close
                );
                window.setFramerateLimit(60);
                if (icon.getSize().x > 0) window.setIcon(icon);

                // Réinitialiser le menuView avec la nouvelle fenêtre
                menuView.rebind(window);
            }
        }
    }

    return 0;
}
