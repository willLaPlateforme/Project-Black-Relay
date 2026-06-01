#include "Affichage/Affichage_Menu/Includes/affichage_menu.hpp"


int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Tower Defense - Black Relay",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Affichage_Menu menuView(window);

// Dans la boucle :
if (model.getState() == GameState::MENU) {
    menuView.handleEvents();  // Gère tout
    menuView.render();
    if (menuView.wantsToPlay()) model.startGame();
    if (menuView.wantsToQuit()) window.close();
}
    }
    return 0;
}
