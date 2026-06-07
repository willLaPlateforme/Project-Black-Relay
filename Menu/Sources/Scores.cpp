#include "../Includes/Scores.hpp"

Scores::Scores(sf::RenderWindow& window, sf::Font& font)
    : _window(&window), _font(font) {}

void Scores::rebind(sf::RenderWindow& window) {
    _window = &window;
}

void Scores::addScore(const std::string& name, int score) {
    _entries.push_back({name, score});
}

void Scores::handleEvents(bool& back) {
    while (auto ev = _window->pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }
        if (auto* k = ev->getIf<sf::Event::KeyPressed>())
            if (k->code == sf::Keyboard::Key::Escape) back = true;
        if (auto* click = ev->getIf<sf::Event::MouseButtonPressed>())
            if (click->button == sf::Mouse::Button::Left) {
                sf::FloatRect backBtn({300.f, 500.f}, {200.f, 40.f});
                if (backBtn.contains(sf::Vector2f(click->position)))
                    back = true;
            }
    }
}

void Scores::render() {
    _window->clear(sf::Color(4, 4, 6));

    sf::Text title(_font);
    title.setCharacterSize(28); title.setLetterSpacing(3.f);
    title.setFillColor(sf::Color(220, 30, 30));
    title.setString("// SCORES");
    title.setPosition({310.f, 100.f});
    _window->draw(title);

    if (_entries.empty()) {
        sf::Text none(_font);
        none.setCharacterSize(16);
        none.setFillColor(sf::Color(90, 90, 90));
        none.setString("Aucun score enregistre.");
        none.setPosition({260.f, 200.f});
        _window->draw(none);
    }

    for (int i = 0; i < (int)_entries.size(); ++i) {
        sf::Text entry(_font);
        entry.setCharacterSize(16);
        entry.setFillColor(sf::Color(180, 180, 180));
        entry.setString(_entries[i].name + "  " + std::to_string(_entries[i].score));
        entry.setPosition({260.f, 180.f + i * 30.f});
        _window->draw(entry);
    }

    sf::Text backTxt(_font);
    backTxt.setCharacterSize(13);
    backTxt.setFillColor(sf::Color(100, 100, 100));
    backTxt.setString("[ESC] RETOUR");
    backTxt.setPosition({360.f, 510.f});
    _window->draw(backTxt);

    _window->display();
}
