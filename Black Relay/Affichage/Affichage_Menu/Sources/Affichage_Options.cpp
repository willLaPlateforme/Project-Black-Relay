#include "../Includes/Affichage_Options.hpp"

Affichage_Options::Affichage_Options(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font) {}

void Affichage_Options::_drawDiffBtn(sf::FloatRect rect, const std::string& label,
                                      Difficulte diff, Difficulte current) {
    bool selected = (current == diff);
    bool hover    = rect.contains(_mousePos);

    sf::RectangleShape btn(rect.size);
    btn.setPosition(rect.position);
    btn.setFillColor(selected
        ? sf::Color(220, 30, 30, 200)
        : sf::Color(0, 0, 0, hover ? 180 : 120));
    btn.setOutlineColor(sf::Color(220, 30, 30, selected ? 255 : (hover ? 150 : 60)));
    btn.setOutlineThickness(1.f);
    _window.draw(btn);

    sf::Text t(_font);
    t.setCharacterSize(13);
    t.setFillColor(selected ? sf::Color(255, 255, 255) : sf::Color(160, 160, 160));
    t.setString(label);
    t.setPosition({rect.position.x + 10.f, rect.position.y + 12.f});
    _window.draw(t);
}

void Affichage_Options::render(const Options& options) {
    _mousePos = sf::Vector2f(sf::Mouse::getPosition(_window));

    _window.clear(sf::Color(4, 4, 6));


    //Titre
    sf::Text title(_font);
    title.setCharacterSize(28);
    title.setLetterSpacing(3.f);
    title.setFillColor(sf::Color(220, 30, 30));
    title.setString("// PARAMETRES");
    title.setPosition({440.f, 120.f});
    _window.draw(title);

    // Separateur
    sf::RectangleShape sep ({600.f, 1.f});
    sep.setFillColor(sf::Color(220, 30, 30, 80));
    sep.setPosition({340.f, 160.f});
    _window.draw(sep);

    // Label son
    sf::Text sonLabel(_font);
    sonLabel.setCharacterSize(14);
    sonLabel.setFillColor(sf::Color(140, 140, 140));
    sonLabel.setString("// SON");
    sonLabel.setPosition({340.f, 220.f});
    _window.draw(sonLabel);

    // Bouton son
    sf::FloatRect soundBtnRect = {{400.f, 250.f}, {240.f, 45.f}};
    bool hoverSound = soundBtnRect.contains(_mousePos);
    sf::RectangleShape soundBtn({240.f, 45.f});
    soundBtn.setPosition({400.f, 250.f});
    soundBtn.setFillColor(options.isSoundEnabled()
        ? sf::Color(30, 80, 30, hoverSound ? 230 : 180)
        : sf::Color(80, 30, 30, hoverSound ? 230 : 180));
    soundBtn.setOutlineColor(sf::Color(220, 30, 30, hoverSound ? 180 : 80));
    soundBtn.setOutlineThickness(1.f);
    _window.draw(soundBtn);

    sf::Text soundTxt(_font);
    soundTxt.setCharacterSize(15);
    soundTxt.setFillColor(sf::Color(220, 220, 220));
    soundTxt.setString(options.isSoundEnabled() ? "SON : ACTIF" : "SON : INACTIF" );
    soundTxt.setPosition({460.f, 262.f});
    _window.draw(soundTxt);

    // Label difficulte
    sf::Text diffLabel(_font);
    diffLabel.setCharacterSize(14);
    diffLabel.setFillColor(sf::Color(140, 140, 140));
    diffLabel.setString("// DIFFICULTE");
    diffLabel.setPosition({340.f, 335.f});
    _window.draw(diffLabel);

    // Boutons difficulte
    Difficulte current = options.getDifficulte();
    _drawDiffBtn({{340.f, 370.f}, {120.f, 40.f}}, "FACILE", Difficulte::FACILE, current);
    _drawDiffBtn({{470.f, 370.f}, {120.f, 40.f}}, "NORMAL", Difficulte::NORMAL, current);
    _drawDiffBtn({{600.f, 370.f}, {120.f, 40.f}}, "DIFFICILE", Difficulte::DIFFICILE, current);
    _drawDiffBtn({{730.f, 370.f}, {120.f, 40.f}}, "ENDLESS", Difficulte::ENDLESS, current);

    // Bouton retour
    sf::FloatRect backBtnRect = {{490.f, 500.f}, {240.f, 45.f}};
    bool hoverBack = backBtnRect.contains(_mousePos);
    sf::RectangleShape backBtn({240.f, 500.f});
    backBtn.setPosition({490.f, 500.f});
    backBtn.setFillColor(sf::Color(0, 0, 0, hoverBack ? 200 : 150));
    backBtn.setOutlineColor(sf::Color(220, 30, 30, hoverBack ? 180 : 80));
    backBtn.setOutlineThickness(1.f);
    _window.draw(backBtn);

    sf::Text backTxt(_font);
    backTxt.setCharacterSize(15);
    backTxt.setFillColor(sf::Color(220, 220, 220));
    backTxt.setString("[ESC] RETOUR");
    backTxt.setPosition({550.f, 512.f});
    _window.draw(backTxt);

    // Scanlines
    for (int y = 0; y < 720; y += 4) {
        sf::RectangleShape line({1280.f, 1.f});
        line.setFillColor(sf::Color(0, 0, 0, 12));
        line.setPosition({0.f, (float)y});
        _window.draw(line);
    }

    // Version
    sf::Text ver(_font);
    ver.setCharacterSize(10);
    ver.setFillColor(sf::Color(70, 70, 70));
    ver.setString("v1.0.0_BETA");
    ver.setPosition({1170.f, 703.f});
    _window.draw(ver);

    _window.display();

}