#include "../Includes/Menu.hpp"

Options::Options(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font), _soundEnabled(true)
{
    _hasBg = _bgTexture.loadFromFile("Assets/Backgrounds/menu_bg.png");
}

void Options::toggleSound() { _soundEnabled = !_soundEnabled; }
bool Options::isSoundEnabled() const { return _soundEnabled; }

void Options::handleEvents(bool& back) {
    while (auto ev = _window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }
        if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape) back = true;
            if (key->code == sf::Keyboard::Key::O) toggleSound();
        }
    }
}

void Options::render() {
    _window.clear(sf::Color(4, 4, 6));

    if (_hasBg) {
        sf::Sprite sp(_bgTexture);
        auto size = _bgTexture.getSize();
        sp.setScale({800.f / size.x, 600.f / size.y});
        _window.draw(sp);
        sf::RectangleShape ov({800.f, 600.f});
        ov.setFillColor(sf::Color(0, 0, 0, 120));
        _window.draw(ov);
    }

    // Scanlines
    for (int y = 0; y < 600; y += 4) {
        sf::RectangleShape line({800.f, 1.f});
        line.setFillColor(sf::Color(0, 0, 0, 12));
        line.setPosition({0.f, (float)y}); _window.draw(line);
    }

    // Panneau centré
    sf::RectangleShape panel({420.f, 280.f});
    panel.setFillColor(sf::Color(0, 0, 0, 185));
    panel.setOutlineColor(sf::Color(220, 30, 30, 160));
    panel.setOutlineThickness(1.f);
    panel.setPosition({190.f, 160.f}); _window.draw(panel);

    sf::RectangleShape topLine({420.f, 2.f});
    topLine.setFillColor(sf::Color(220, 30, 30, 200));
    topLine.setPosition({190.f, 160.f}); _window.draw(topLine);

    auto txt = [&](const std::string& s, float x, float y, sf::Color c, unsigned sz = 14) {
        sf::Text t(_font); t.setCharacterSize(sz); t.setFillColor(c);
        t.setString(s); t.setPosition({x, y}); _window.draw(t);
    };
    txt("// PARAMETRES", 210.f, 178.f, sf::Color(220,30,30), 22);
    txt("SON :", 210.f, 240.f, sf::Color(160,160,160));
    txt(_soundEnabled ? "ACTIF" : "INACTIF", 290.f, 240.f,
        _soundEnabled ? sf::Color(80,220,80) : sf::Color(180,60,60));
    txt("[O] pour basculer", 210.f, 262.f, sf::Color(80,80,80), 11);
    txt("[ESC]  RETOUR", 210.f, 400.f, sf::Color(90,90,90), 13);

    _window.display();
}
