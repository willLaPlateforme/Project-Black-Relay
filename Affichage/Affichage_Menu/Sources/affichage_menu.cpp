#include "../Includes/affichage_menu.hpp"
#include "../../../Menu/Includes/Menu.hpp"

Affichage_Menu::Affichage_Menu(sf::RenderWindow& window)
    : _window(window), _menuState(MenuState::MAIN),
      _play(false), _quit(false), _soundEnabled(true)
{
    _font.openFromFile("Assets/Fonts/font.ttf");
}

void Affichage_Menu::handleEvents() {
    _play = false; _quit = false;

    Menu menu(_window);

    // Boucle du menu jusqu'à une action
    while (_window.isOpen() && !_play && !_quit) {
        menu.handleEvents();
        menu.render();

        int action = menu.getActivated();
        if (action < 0) continue;
        menu.resetActivated();

        switch (action) {
            case Menu::CONTINUER:
            case Menu::NOUVELLE_PARTIE:
                _play = true;
                break;
            case Menu::PARAMETRES: {
                Options opt(_window, _font);
                bool back = false;
                while (_window.isOpen() && !back) {
                    opt.handleEvents(back);
                    opt.render();
                }
                _soundEnabled = opt.isSoundEnabled();
                break;
            }
            case Menu::CREDITS: {
                // Écran crédits simple
                bool back = false;
                while (_window.isOpen() && !back) {
                    while (auto ev = _window.pollEvent()) {
                        if (ev->is<sf::Event::Closed>()) { back = true; _quit = true; }
                        if (auto* k = ev->getIf<sf::Event::KeyPressed>())
                            if (k->code == sf::Keyboard::Key::Escape) back = true;
                    }
                    _window.clear(sf::Color(4, 4, 6));
                    sf::Text t(_font);
                    t.setCharacterSize(20);
                    t.setFillColor(sf::Color(220, 30, 30));
                    t.setString("// CREDITS\n\nProjet Tower Defense\nLa Plateforme 2025\n\n[ESC] Retour");
                    t.setPosition({250.f, 200.f});
                    _window.draw(t);
                    _window.display();
                }
                break;
            }
            case Menu::QUITTER:
                _quit = true;
                break;
        }
    }
}

void Affichage_Menu::render() {
    // Appelé uniquement pour le premier frame — la boucle est dans handleEvents
}

bool Affichage_Menu::wantsToPlay() const { return _play; }
bool Affichage_Menu::wantsToQuit() const { return _quit; }
bool Affichage_Menu::isSoundOn()   const { return _soundEnabled; }
