#include <SFML/Graphics.hpp>
#include "Affichage/Affichage_Menu/Includes/affichage_menu.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 750}), "Black Relay",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Chargement de l'icone de la fenetre depuis un PNG
    sf::Image icon;
    if (icon.loadFromFile("Assets/Icones/Ico/Black Relay.png"))
        window.setIcon(icon);

    Affichage_Menu menuView(window);

    while (window.isOpen()) {
        menuView.handleEvents();
        menuView.render();

        if (menuView.wantsToQuit()) window.close();
        if (menuView.wantsToPlay()) {
            // TODO : lancer le jeu ici
        }
    }
    return 0;
}