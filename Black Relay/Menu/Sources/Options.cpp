#include "../Includes/Options.hpp"

Options::Options(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font), _soundEnabled(true) {}

void Options::handleEvents(bool& back) {
    _mousePos = sf::Vector2f(sf::Mouse::getPosition(_window));

    while (auto ev = _window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }

        if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::Escape) back = true;
            if (k->code == sf::Keyboard::Key::S) _soundEnabled = !_soundEnabled;
        }

        if (auto* click = ev->getIf<sf::Event::MouseButtonPressed>()) {
            if (click->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos(click->position);
                if (_soundBtnRect.contains(pos)) _soundEnabled = !_soundEnabled;
                if (_backBtnRect.contains(pos))  back = true;
            }
        }
    }
}

void Options::render() {
    _window.clear(sf::Color(4, 4, 6));

    sf::Text title(_font);
    title.setCharacterSize(28); title.setLetterSpacing(3.f);
    title.setFillColor(sf::Color(220, 30, 30));
    title.setString("// PARAMETRES");
    title.setPosition({260.f, 150.f});
    _window.draw(title);

    // Bouton son — change de couleur selon état + hover
    bool hoverSound = _soundBtnRect.contains(_mousePos);
    sf::RectangleShape soundBtn({200.f, 40.f});
    soundBtn.setPosition({300.f, 250.f});
    soundBtn.setFillColor(_soundEnabled
        ? sf::Color(30, 80, 30, hoverSound ? 220 : 180)
        : sf::Color(80, 30, 30, hoverSound ? 220 : 180));
    soundBtn.setOutlineColor(sf::Color(220, 30, 30, 100));
    soundBtn.setOutlineThickness(1.f);
    _window.draw(soundBtn);

    sf::Text soundTxt(_font);
    soundTxt.setCharacterSize(15);
    soundTxt.setFillColor(sf::Color(220, 220, 220));
    soundTxt.setString(_soundEnabled ? "SON : ACTIF" : "SON : INACTIF");
    soundTxt.setPosition({320.f, 260.f});
    _window.draw(soundTxt);

    // Bouton retour
    bool hoverBack = _backBtnRect.contains(_mousePos);
    sf::RectangleShape backBtn({200.f, 40.f});
    backBtn.setPosition({300.f, 400.f});
    backBtn.setFillColor(sf::Color(0, 0, 0, hoverBack ? 220 : 180));
    backBtn.setOutlineColor(sf::Color(220, 30, 30, hoverBack ? 180 : 100));
    backBtn.setOutlineThickness(1.f);
    _window.draw(backBtn);

    sf::Text backTxt(_font);
    backTxt.setCharacterSize(15);
    backTxt.setFillColor(sf::Color(220, 220, 220));
    backTxt.setString("[ESC] RETOUR");
    backTxt.setPosition({320.f, 410.f});
    _window.draw(backTxt);

    _window.display();
}

bool Options::isSoundEnabled() const { return _soundEnabled; }