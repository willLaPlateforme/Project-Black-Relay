#include "../Includes/affichage_menu.hpp"
#include "../../../Menu/Includes/Menu.hpp"

// ── Constructeur ──────────────────────────────────────────────────────────────
Affichage_Menu::Affichage_Menu(sf::RenderWindow& window)
    : _window(window), _menuState(MenuState::MAIN),
      _play(false), _quit(false), _soundEnabled(true)
{
    _font.openFromFile("Assets/Fonts/font.ttf");
}

// ── Boucle de gestion du menu ─────────────────────────────────────────────────
void Affichage_Menu::handleEvents() {
    _play = false;
    _quit = false;

    Menu menu(_window);

    // Boucle jusqu'à une action utilisateur
    while (_window.isOpen() && !_play && !_quit) {
        menu.handleEvents();
        menu.render();

        int action = menu.getActivated();
        if (action < 0) continue;
        menu.resetActivated();

        switch (action) {

            // Continuer ou nouvelle partie : lancer le jeu
            case Menu::CONTINUER:
            case Menu::NOUVELLE_PARTIE:
                _play = true;
                break;

            // Paramètres : écran son on/off
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

            // Scores : tableau des meilleurs scores
            case Menu::SCORES: {
                Scores scores(_window, _font);
                bool back = false;
                while (_window.isOpen() && !back) {
                    scores.handleEvents(back);
                    scores.render();
                }
                break;
            }

            // Crédits : écran simple
            case Menu::CREDITS: {
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
                    t.setString("// CREDITS\n\nProjet Tower Defense - Black Relay\nLa Plateforme 2025\n\n[ESC] Retour");
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

void Affichage_Menu::render() {}

bool Affichage_Menu::wantsToPlay() const { return _play; }
bool Affichage_Menu::wantsToQuit() const { return _quit; }
bool Affichage_Menu::isSoundOn()   const { return _soundEnabled; }

// ── Sauvegarde d'un score depuis le jeu ──────────────────────────────────────
void Affichage_Menu::saveScore(const std::string& name, int score) {
    Scores scores(_window, _font);
    scores.addScore(name, score);
}
