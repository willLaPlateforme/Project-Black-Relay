#include "../Includes/Menu.hpp"
#include <algorithm>

Scores::Scores(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font) {}

void Scores::addScore(const std::string& name, int score) {
    _entries.push_back({name, score});
    std::sort(_entries.begin(), _entries.end(),
        [](const Entry& a, const Entry& b){ return a.score > b.score; });
    if (_entries.size() > 10) _entries.resize(10);
}

void Scores::handleEvents(bool& back) {
    while (auto ev = _window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }
        if (auto* key = ev->getIf<sf::Event::KeyPressed>())
            if (key->code == sf::Keyboard::Key::Escape) back = true;
    }
}

void Scores::render() {
    _window.clear(sf::Color(4, 4, 6));

    sf::RectangleShape panel({500.f, 420.f});
    panel.setFillColor(sf::Color(5, 5, 8, 230));
    panel.setOutlineColor(sf::Color(220, 30, 30, 150));
    panel.setOutlineThickness(1.f);
    panel.setPosition({150.f, 80.f});
    _window.draw(panel);

    sf::Text title(_font);
    title.setCharacterSize(24);
    title.setFillColor(sf::Color(220, 30, 30));
    title.setLetterSpacing(3.f);
    title.setString("// MEILLEURS COMMANDANTS");
    title.setPosition({170.f, 100.f});
    _window.draw(title);

    sf::RectangleShape sep({460.f, 1.f});
    sep.setFillColor(sf::Color(220, 30, 30, 80));
    sep.setPosition({170.f, 135.f});
    _window.draw(sep);

    if (_entries.empty()) {
        sf::Text empty(_font);
        empty.setCharacterSize(16);
        empty.setFillColor(sf::Color(80, 80, 80));
        empty.setString("// AUCUN ENREGISTREMENT");
        empty.setPosition({230.f, 260.f});
        _window.draw(empty);
    } else {
        for (int i = 0; i < (int)_entries.size(); ++i) {
            bool top = (i == 0);
            sf::Text line(_font);
            line.setCharacterSize(16);
            line.setFillColor(top ? sf::Color(220, 30, 30) : sf::Color(160, 160, 160));
            line.setString(std::to_string(i+1) + ".  " + _entries[i].name +
                           "   " + std::to_string(_entries[i].score) + " pts");
            line.setPosition({170.f, 150.f + i * 30.f});
            _window.draw(line);
        }
    }

    sf::Text back(_font);
    back.setCharacterSize(13);
    back.setFillColor(sf::Color(80, 80, 80));
    back.setString("[ESC]  RETOUR");
    back.setPosition({170.f, 460.f});
    _window.draw(back);

    _window.display();
}
