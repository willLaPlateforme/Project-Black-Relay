#include "../Includes/Menu.hpp"
#include <cmath>

// Helper : dessine le background + overlay
static void drawBg(sf::RenderWindow& window, sf::Texture& tex, bool hasBg) {
    if (hasBg) {
        sf::Sprite sp(tex);
        auto size = tex.getSize();
        sp.setScale({800.f / size.x, 600.f / size.y});
        window.draw(sp);
        sf::RectangleShape ov({800.f, 600.f});
        ov.setFillColor(sf::Color(0, 0, 0, 70));
        window.draw(ov);
    } else {
        sf::RectangleShape bg({800.f, 600.f});
        bg.setFillColor(sf::Color(4, 4, 6));
        window.draw(bg);
    }
}

Menu::Menu(sf::RenderWindow& window)
    : _window(window), _selectedIndex(0), _activatedIndex(-1), _animTimer(0.f)
{
    _font.openFromFile("Assets/Fonts/font.ttf");
    _hasBg = _bgTexture.loadFromFile("Assets/Backgrounds/menu_bg.png");

    _items = {{"CONTINUER"},{"NOUVELLE PARTIE"},{"PARAMETRES"},{"CREDITS"},{"QUITTER"}};

    _leftPanel.setSize({300.f, 600.f});
    _leftPanel.setFillColor(sf::Color(0, 0, 0, 155));
    _leftPanel.setPosition({0.f, 0.f});

    _selectBar.setSize({4.f, 36.f});

    _newsPanel.setSize({285.f, 88.f});
    _newsPanel.setFillColor(sf::Color(0, 0, 0, 170));
    _newsPanel.setOutlineColor(sf::Color(220, 30, 30, 130));
    _newsPanel.setOutlineThickness(1.f);
    _newsPanel.setPosition({8.f, 492.f});
}

void Menu::handleEvents() {
    _animTimer += 0.016f;
    while (auto ev = _window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { _activatedIndex = QUITTER; return; }
        if (auto* key = ev->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Up:
                    _selectedIndex = (_selectedIndex - 1 + (int)_items.size()) % (int)_items.size(); break;
                case sf::Keyboard::Key::Down:
                    _selectedIndex = (_selectedIndex + 1) % (int)_items.size(); break;
                case sf::Keyboard::Key::Enter: _activatedIndex = _selectedIndex; break;
                case sf::Keyboard::Key::Escape: _activatedIndex = QUITTER; break;
                default: break;
            }
        }
    }
}

void Menu::render() {
    _window.clear(sf::Color(4, 4, 6));
    _drawBackground();
    _window.draw(_leftPanel);
    _drawMenuItems();
    _drawStatusPanel();
    _window.draw(_newsPanel);
    _drawNewsPanel();
    _drawScanlines();
    _drawCornerDeco();
    _window.display();
}

int  Menu::getActivated()   const { return _activatedIndex; }
void Menu::resetActivated()       { _activatedIndex = -1; }

void Menu::_drawBackground() {
    drawBg(_window, _bgTexture, _hasBg);
    sf::RectangleShape sep({1.f, 600.f});
    sep.setFillColor(sf::Color(220, 30, 30, 70));
    sep.setPosition({300.f, 0.f});
    _window.draw(sep);
}

void Menu::_drawMenuItems() {
    float startY = 300.f, itemH = 42.f;
    for (int i = 0; i < (int)_items.size(); ++i) {
        bool sel = (i == _selectedIndex);
        float y = startY + i * itemH;
        if (sel) {
            sf::RectangleShape bg({265.f, 34.f});
            bg.setFillColor(sf::Color(220, 30, 30, 40));
            bg.setPosition({25.f, y - 4.f});
            _window.draw(bg);
            float pulse = 0.7f + 0.3f * std::sin(_animTimer * 4.f);
            _selectBar.setFillColor(sf::Color(220, 30, 30, (uint8_t)(255 * pulse)));
            _selectBar.setPosition({18.f, y - 2.f});
            _window.draw(_selectBar);
            sf::Text arrow(_font); arrow.setCharacterSize(14);
            arrow.setFillColor(sf::Color(220, 30, 30));
            arrow.setString(">"); arrow.setPosition({26.f, y + 4.f});
            _window.draw(arrow);
        } else {
            sf::CircleShape dot(2.5f);
            dot.setFillColor(sf::Color(90, 90, 90));
            dot.setPosition({29.f, y + 10.f});
            _window.draw(dot);
        }
        sf::Text label(_font); label.setCharacterSize(17);
        label.setLetterSpacing(2.f);
        label.setFillColor(sel ? sf::Color(235,235,235) : sf::Color(110,110,110));
        label.setString(_items[i].label);
        label.setPosition({42.f, y});
        _window.draw(label);
    }
}

void Menu::_drawStatusPanel() {
    sf::RectangleShape panel({195.f, 80.f});
    panel.setFillColor(sf::Color(0, 0, 0, 170));
    panel.setOutlineColor(sf::Color(220, 30, 30, 80));
    panel.setOutlineThickness(1.f);
    panel.setPosition({596.f, 14.f});
    _window.draw(panel);
    auto txt = [&](const std::string& s, float x, float y, sf::Color c, unsigned sz = 10) {
        sf::Text t(_font); t.setCharacterSize(sz); t.setFillColor(c);
        t.setString(s); t.setPosition({x, y}); _window.draw(t);
    };
    txt("// SYSTEME BLACK RELAY", 602.f, 22.f, sf::Color(140,140,140));
    txt("STATUT :", 602.f, 38.f, sf::Color(140,140,140));
    txt("ACTIF",   650.f, 38.f, sf::Color(80,220,80));
    txt("INTEGRITE :", 602.f, 54.f, sf::Color(140,140,140));
    txt("87.6%",       672.f, 54.f, sf::Color(220,30,30));
    txt("[FLECHES] Nav   [ENTREE] Valider", 602.f, 72.f, sf::Color(70,70,70), 9);
}

void Menu::_drawNewsPanel() {
    sf::Text tag(_font); tag.setCharacterSize(10);
    tag.setFillColor(sf::Color(140,140,140)); tag.setString("// NEWS_FEED");
    tag.setPosition({12.f, 480.f}); _window.draw(tag);
    sf::Text alert(_font); alert.setCharacterSize(11);
    alert.setFillColor(sf::Color(220,30,30)); alert.setStyle(sf::Text::Bold);
    alert.setString("ALERTE GLOBALE"); alert.setPosition({16.f, 500.f}); _window.draw(alert);
    sf::Text news(_font); news.setCharacterSize(10);
    news.setFillColor(sf::Color(140,140,140));
    news.setString("Activite ennemie en hausse.\nRestez vigilants, Commandant.");
    news.setPosition({16.f, 518.f}); _window.draw(news);
}

void Menu::_drawScanlines() {
    for (int y = 0; y < 600; y += 4) {
        sf::RectangleShape line({800.f, 1.f});
        line.setFillColor(sf::Color(0, 0, 0, 12));
        line.setPosition({0.f, (float)y}); _window.draw(line);
    }
    sf::Text ver(_font); ver.setCharacterSize(10);
    ver.setFillColor(sf::Color(70,70,70)); ver.setString("v1.0.0_BETA");
    ver.setPosition({718.f, 583.f}); _window.draw(ver);
}

void Menu::_drawCornerDeco() {
    auto corner = [&](float x, float y, float sx, float sy) {
        sf::RectangleShape h({18.f, 1.f});
        h.setFillColor(sf::Color(220,30,30,100)); h.setPosition({x,y}); h.setScale({sx,1.f});
        _window.draw(h);
        sf::RectangleShape v({1.f, 18.f});
        v.setFillColor(sf::Color(220,30,30,100)); v.setPosition({x,y}); v.setScale({1.f,sy});
        _window.draw(v);
    };
    corner(2.f,2.f,1.f,1.f); corner(798.f,2.f,-1.f,1.f);
    corner(2.f,598.f,1.f,-1.f); corner(798.f,598.f,-1.f,-1.f);
}
