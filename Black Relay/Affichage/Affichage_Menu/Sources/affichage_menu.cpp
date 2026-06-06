#include "../Includes/affichage_menu.hpp"
#include "../Includes/Affichage_Options.hpp"
#include <cmath>

Affichage_Menu::Affichage_Menu(sf::RenderWindow& window)
    : _window(window), _menuState(MenuState::MAIN),
      _play(false), _quit(false), _soundEnabled(true),
      _animTimer(0.f)
{
    _font.openFromFile("Assets/Fonts/font.ttf");
    _backgroundTexture.loadFromFile("Assets/Backgrounds/Menu_Background/Menu_background.png");
    _backgroundSprite.emplace(_backgroundTexture);

    _leftPanel.setSize({320.f, 420.f});
    _leftPanel.setFillColor(sf::Color(0, 0, 0, 0));
    _leftPanel.setPosition({0.f, 280.f});

    _selectBar.setSize({4.f, 30.f});

    _newsPanel.setSize({310.f, 80.f});
    _newsPanel.setFillColor(sf::Color(0, 0, 0, 170));
    _newsPanel.setOutlineColor(sf::Color(220, 30, 30, 130));
    _newsPanel.setOutlineThickness(1.f);
    _newsPanel.setPosition({8.f, 638.f});
}

void Affichage_Menu::handleEvents() {
    _play = false; _quit = false;

    Menu menu;

    while (_window.isOpen() && !_play && !_quit) {
        _animTimer += 0.016f;
        _mousePos = sf::Vector2f(sf::Mouse::getPosition(_window));
        menu.update(_animTimer, _mousePos);

        while (auto ev = _window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                menu.handleClosed();
            }
            if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                menu.handleKeyPressed(key->code);
            }
            if (ev->is<sf::Event::MouseMoved>()) {
                menu.handleMouseMoved(_mousePos);
            }
            if (auto* click = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left)
                    menu.handleMouseClicked(sf::Vector2f(click->position));
            }
        }

        _window.clear(sf::Color(4, 4, 6));
        _drawBackground();
        _window.draw(_leftPanel);
        _drawMenuItems(menu);
        _drawStatusPanel();
        _window.draw(_newsPanel);
        _drawNewsPanel();
        _drawScanlines();
        _drawCornerDeco();
        _window.display();

        int action = menu.getActivated();
        if (action < 0) continue;
        menu.resetActivated();

        switch (action) {
            case Menu::CONTINUER:
            case Menu::NOUVELLE_PARTIE:
                _play = true;
                break;
            case Menu::OPERATEURS:
            case Menu::ARSENAL:
            case Menu::RECHERCHE:
            case Menu::DOSSIER:
                // TODO : implementer ces ecrans
                break;
            case Menu::PARAMETRES: {
                Options opt(_window, _font, menu);
                Affichage_Options affOpt(_window, _font);
                bool back = false;
                while (_window.isOpen() && !back) {
                    opt.handleEvents(back);
                    affOpt.render(opt);
                }
                _soundEnabled = opt.isSoundEnabled();
                _difficulte   = opt.getDifficulte();
                break;
            }
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
    _window.clear();
    if (_backgroundSprite.has_value())
        _window.draw(_backgroundSprite.value());
    _window.display();
}

void Affichage_Menu::_drawBackground() {
    if (_backgroundSprite.has_value()) {
        auto size = _backgroundTexture.getSize();
        _backgroundSprite->setScale({1280.f / size.x, 720.f / size.y});
        _window.draw(_backgroundSprite.value());
        sf::RectangleShape ov({1280.f, 720.f});
        ov.setFillColor(sf::Color(0, 0, 0, 70));
        _window.draw(ov);
    } else {
        sf::RectangleShape bg({1280.f, 720.f});
        bg.setFillColor(sf::Color(4, 4, 6));
        _window.draw(bg);
    }
    sf::RectangleShape sep({1.f, 440.f});
    sep.setFillColor(sf::Color(220, 30, 30, 70));
    sep.setPosition({320.f, 280.f});
    _window.draw(sep);
}

void Affichage_Menu::_drawMenuItems(const Menu& menu) {
    float startY = 295.f, itemH = 35.f;
    const auto& items = menu.getItems();
    int sel = menu.getSelectedIndex();
    int hov = menu.getHoveredIndex();

    for (int i = 0; i < (int)items.size(); ++i) {
        bool isSel = (i == sel);
        bool isHov = (i == hov);
        float y = startY + i * itemH;

        if (isSel) {
            sf::RectangleShape bg({285.f, 30.f});
            bg.setFillColor(sf::Color(220, 30, 30, 40));
            bg.setPosition({25.f, y - 4.f});
            _window.draw(bg);
            float pulse = 0.7f + 0.3f * std::sin(_animTimer * 4.f);
            _selectBar.setFillColor(sf::Color(220, 30, 30, (uint8_t)(255 * pulse)));
            _selectBar.setPosition({18.f, y - 2.f});
            _window.draw(_selectBar);
            sf::Text arrow(_font); arrow.setCharacterSize(14);
            arrow.setFillColor(sf::Color(220, 30, 30));
            arrow.setString(">"); arrow.setPosition({26.f, y + 2.f});
            _window.draw(arrow);
        } else if (isHov) {
            sf::RectangleShape bg({285.f, 30.f});
            bg.setFillColor(sf::Color(220, 30, 30, 15));
            bg.setPosition({25.f, y - 4.f});
            _window.draw(bg);
        } else {
            sf::CircleShape dot(2.f);
            dot.setFillColor(sf::Color(90, 90, 90));
            dot.setPosition({29.f, y + 8.f});
            _window.draw(dot);
        }

        sf::Text label(_font); label.setCharacterSize(17);
        label.setLetterSpacing(2.f);
        label.setFillColor(isSel ? sf::Color(235,235,235) : (isHov ? sf::Color(180,180,180) : sf::Color(110,110,110)));
        label.setString(items[i].label);
        label.setPosition({44.f, y});
        _window.draw(label);
    }
}

void Affichage_Menu::_drawStatusPanel() {
    sf::RectangleShape panel({195.f, 90.f});
    panel.setFillColor(sf::Color(0, 0, 0, 170));
    panel.setOutlineColor(sf::Color(220, 30, 30, 80));
    panel.setOutlineThickness(1.f);
    panel.setPosition({1075.f, 12.f});
    _window.draw(panel);
    auto txt = [&](const std::string& s, float x, float y, sf::Color c, unsigned sz = 11) {
        sf::Text t(_font); t.setCharacterSize(sz); t.setFillColor(c);
        t.setString(s); t.setPosition({x, y}); _window.draw(t);
    };
    txt("// SYSTEME BLACK RELAY", 1081.f, 20.f, sf::Color(140,140,140));
    txt("STATUT :",        1081.f, 36.f, sf::Color(140,140,140));
    txt("ACTIF",           1135.f, 36.f, sf::Color(80,220,80));
    txt("COEUR DU RELAIS", 1081.f, 52.f, sf::Color(140,140,140));
    txt("INTEGRITE :",     1081.f, 66.f, sf::Color(140,140,140));
    txt("87.6%",           1155.f, 66.f, sf::Color(220,30,30));
}

void Affichage_Menu::_drawNewsPanel() {
    sf::Text tag(_font); tag.setCharacterSize(11);
    tag.setFillColor(sf::Color(140,140,140)); tag.setString("// NEWS_FEED");
    tag.setPosition({12.f, 624.f}); _window.draw(tag);
    sf::Text alert(_font); alert.setCharacterSize(13);
    alert.setFillColor(sf::Color(220,30,30)); alert.setStyle(sf::Text::Bold);
    alert.setString("ALERTE GLOBALE"); alert.setPosition({76.f, 642.f}); _window.draw(alert);
    sf::Text news(_font); news.setCharacterSize(11);
    news.setFillColor(sf::Color(140,140,140));
    news.setString("Activite ennemie en hausse.\nRestez vigilants, Commandant.");
    news.setPosition({76.f, 658.f}); _window.draw(news);
}

void Affichage_Menu::_drawScanlines() {
    for (int y = 0; y < 720; y += 4) {
        sf::RectangleShape line({1280.f, 1.f});
        line.setFillColor(sf::Color(0, 0, 0, 12));
        line.setPosition({0.f, (float)y}); _window.draw(line);
    }
    sf::Text ver(_font); ver.setCharacterSize(10);
    ver.setFillColor(sf::Color(70,70,70)); ver.setString("v1.0.0_BETA");
    ver.setPosition({1170.f, 703.f}); _window.draw(ver);
}

void Affichage_Menu::_drawCornerDeco() {
    auto corner = [&](float x, float y, float sx, float sy) {
        sf::RectangleShape h({18.f, 1.f});
        h.setFillColor(sf::Color(220,30,30,100)); h.setPosition({x,y}); h.setScale({sx,1.f});
        _window.draw(h);
        sf::RectangleShape v({1.f, 18.f});
        v.setFillColor(sf::Color(220,30,30,100)); v.setPosition({x,y}); v.setScale({1.f,sy});
        _window.draw(v);
    };
    corner(2.f,2.f,1.f,1.f);       corner(1278.f,2.f,-1.f,1.f);
    corner(2.f,718.f,1.f,-1.f);    corner(1278.f,718.f,-1.f,-1.f);
}

bool Affichage_Menu::wantsToPlay() const { return _play; }
bool Affichage_Menu::wantsToQuit() const { return _quit; }
bool Affichage_Menu::isSoundOn()   const { return _soundEnabled; }
Difficulte Affichage_Menu::getDifficulte() const { return _difficulte; }