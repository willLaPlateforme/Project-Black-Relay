#include "../Includes/Options.hpp"
#include "../Includes/Menu.hpp"

Options::Options(sf::RenderWindow& window, sf::Font& font, Menu& menu)
    : _window(window), _font(font), _menu(menu) {}

void Options::handleEvents(bool& back) {
    _mousePos = sf::Vector2f(sf::Mouse::getPosition(_window));

    while (auto ev = _window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Escape) back = true;
            if (k->code == sf::Keyboard::Key::S) {
                _soundEnabled = !_soundEnabled;
                _menu.toggleSound();
            }
        }

        if (auto* click = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (click->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos(click->position);

                if (_soundBtnRect.contains(pos)) {
                    _soundEnabled = !_soundEnabled;
                    _menu.toggleSound();
                }

                if (_facileRect.contains(pos))    _difficulte = Difficulte::FACILE;
                if (_normalRect.contains(pos))    _difficulte = Difficulte::NORMAL;
                if (_difficileRect.contains(pos)) _difficulte = Difficulte::DIFFICILE;
                if (_endlessRect.contains(pos))   _difficulte = Difficulte::ENDLESS;

                if (_backBtnRect.contains(pos)) back = true;
            }
        }
    }
}

bool       Options::isSoundEnabled() const { return _soundEnabled; }
Difficulte Options::getDifficulte()  const { return _difficulte; }