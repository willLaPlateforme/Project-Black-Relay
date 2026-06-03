#include "../Includes/Game.hpp"

GameView::GameView(sf::RenderWindow& window) : _window(window) {
    _font.openFromFile("Assets/Fonts/font.ttf");

    _scoreText.emplace(_font);
    _resourceText.emplace(_font);
    _baseHpText.emplace(_font);
    _waveText.emplace(_font);
    _towerInfoText.emplace(_font);

    // Chargement de la map en background
    _hasMapBg = _mapTexture.loadFromFile("Assets/Backgrounds/map_bg.png");
    if (_hasMapBg) {
        _mapSprite.emplace(_mapTexture);
        auto size = _mapTexture.getSize();
        // Mise à l'échelle pour couvrir 800x550 (zone de jeu sous le HUD)
        _mapSprite->setScale({800.f / size.x, 550.f / size.y});
        _mapSprite->setPosition({0.f, 50.f});
    }

    _setupHUD();
}

void GameView::onEvent(const std::string& event, int value) {
    if      (event == "SCORE")     _scoreText->setString("Score: "     + std::to_string(value));
    else if (event == "RESOURCES") _resourceText->setString("Shards: " + std::to_string(value));
    else if (event == "BASE_HP")   _baseHpText->setString("Relais: "   + std::to_string(value) + " HP");
    else if (event == "WAVE")      _waveText->setString("Vague: "      + std::to_string(value) + "/10");
}

void GameView::render(Game& game) {
    _window.clear(sf::Color(14, 14, 20));
    switch (game.getState()) {
        case GameState::PLAYING: _drawGame(game);                       break;
        case GameState::PAUSED:  _drawGame(game); _drawPause();         break;
        case GameState::VICTORY: _drawEndScreen(true);                  break;
        case GameState::DEFEAT:  _drawEndScreen(false);                 break;
        default: break;
    }
    _window.display();
}

void GameView::_setupHUD() {
    _hudBar.setSize({800.f, 50.f});
    _hudBar.setFillColor(sf::Color(10, 10, 14, 220));
    auto init = [&](std::optional<sf::Text>& t, float x, const std::string& s) {
        t->setCharacterSize(16); t->setFillColor(sf::Color::White);
        t->setPosition({x, 14.f}); t->setString(s);
    };
    init(_scoreText,    10.f,  "Score: 0");
    init(_resourceText, 190.f, "Shards: 200");
    init(_baseHpText,   390.f, "Relais: 20 HP");
    init(_waveText,     570.f, "Vague: 0/10");
}

void GameView::_drawGame(Game& game) {

    // ── 1. IMAGE DE LA MAP EN FOND ──────────────────────────────────────────
    if (_hasMapBg && _mapSprite) {
        _window.draw(*_mapSprite);
        // Légère couche sombre pour que les ennemis/tours restent lisibles
        sf::RectangleShape overlay({800.f, 550.f});
        overlay.setPosition({0.f, 50.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 60));
        _window.draw(overlay);
    } else {
        // Fallback si pas d'image : fond sombre + grille
        _drawGrid();
        _drawPath(game.getPath());
        _drawSlots(game.getSlots());
    }

    // ── 2. SLOTS (transparents, juste pour indiquer au joueur) ──────────────
    // On dessine les slots par-dessus l'image mais discrets
    _drawSlotsOverlay(game.getSlots());

    // ── 3. BLACK RELAY (objectif) ────────────────────────────────────────────
    _drawBlackRelay(game.getPath());

    // ── 4. TOURS ─────────────────────────────────────────────────────────────
    for (auto& t : game.getTowers()) {
        t->draw(_window, false);
        sf::Text name(_font);
        name.setCharacterSize(9);
        name.setFillColor(sf::Color(220, 210, 80));
        name.setString(t->getName() + " " + std::string(t->getLevel(), '*'));
        sf::Vector2f pos = t->getPosition();
        name.setPosition({pos.x - 18.f, pos.y - 30.f});
        _window.draw(name);
    }

    // ── 5. ENNEMIS ────────────────────────────────────────────────────────────
    for (auto& e : game.getEnemies()) {
        e->draw(_window);
        sf::Text name(_font);
        name.setCharacterSize(9);
        name.setFillColor(sf::Color(220, 100, 100));
        name.setString(e->getId());
        sf::Vector2f pos = e->getPosition();
        name.setPosition({pos.x - 14.f, pos.y - 30.f});
        _window.draw(name);
    }

    // ── 6. HUD PAR DESSUS TOUT ────────────────────────────────────────────────
    _drawHUD(game);
}

void GameView::_drawGrid() {
    for (int x = 0; x <= 800; x += 40) {
        sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f((float)x, 50.f),  sf::Color(255,255,255,10)},
            sf::Vertex{sf::Vector2f((float)x, 600.f), sf::Color(255,255,255,10)}
        };
        _window.draw(line, 2, sf::PrimitiveType::Lines);
    }
    for (int y = 50; y <= 600; y += 40) {
        sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f(0.f,   (float)y), sf::Color(255,255,255,10)},
            sf::Vertex{sf::Vector2f(800.f, (float)y), sf::Color(255,255,255,10)}
        };
        _window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

void GameView::_drawPath(const std::vector<sf::Vector2f>& path) {
    if (path.size() < 2) return;
    const float hw = 18.f;
    for (int i = 0; i + 1 < (int)path.size(); ++i) {
        sf::Vector2f a = path[i], b = path[i+1];
        float dx = b.x - a.x, dy = b.y - a.y;
        if (std::hypot(dx, dy) < 1.f) continue;
        if (std::abs(dx) > std::abs(dy)) {
            float x0 = std::min(a.x, b.x), x1 = std::max(a.x, b.x);
            sf::RectangleShape bg({x1-x0, hw*2});
            bg.setPosition({x0, a.y - hw});
            bg.setFillColor(sf::Color(40, 10, 10, 180));
            _window.draw(bg);
            sf::RectangleShape top({x1-x0, 2.f});
            top.setPosition({x0, a.y - hw});
            top.setFillColor(sf::Color(180, 30, 30, 140));
            _window.draw(top);
            sf::RectangleShape bot({x1-x0, 2.f});
            bot.setPosition({x0, a.y + hw - 2.f});
            bot.setFillColor(sf::Color(180, 30, 30, 140));
            _window.draw(bot);
        } else {
            float y0 = std::min(a.y, b.y), y1 = std::max(a.y, b.y);
            sf::RectangleShape bg({hw*2, y1-y0});
            bg.setPosition({a.x - hw, y0});
            bg.setFillColor(sf::Color(40, 10, 10, 180));
            _window.draw(bg);
            sf::RectangleShape left({2.f, y1-y0});
            left.setPosition({a.x - hw, y0});
            left.setFillColor(sf::Color(180, 30, 30, 140));
            _window.draw(left);
            sf::RectangleShape right({2.f, y1-y0});
            right.setPosition({a.x + hw - 2.f, y0});
            right.setFillColor(sf::Color(180, 30, 30, 140));
            _window.draw(right);
        }
    }
}

// Slots discrets par-dessus l'image (juste la croix, pas de fond)
void GameView::_drawSlotsOverlay(const std::vector<Slot>& slots) {
    for (const auto& s : slots) {
        if (s.occupied) continue; // slot occupé = tour visible, pas besoin de croix

        sf::Color col(160, 160, 200, 140);

        sf::RectangleShape h({18.f, 2.f});
        h.setOrigin({9.f, 1.f});
        h.setPosition(s.pos);
        h.setFillColor(col);
        _window.draw(h);

        sf::RectangleShape v({2.f, 18.f});
        v.setOrigin({1.f, 9.f});
        v.setPosition(s.pos);
        v.setFillColor(col);
        _window.draw(v);

        // Petit cadre autour
        sf::RectangleShape frame({28.f, 28.f});
        frame.setOrigin({14.f, 14.f});
        frame.setPosition(s.pos);
        frame.setFillColor(sf::Color::Transparent);
        frame.setOutlineColor(sf::Color(140, 140, 180, 100));
        frame.setOutlineThickness(0.8f);
        _window.draw(frame);
    }
}

void GameView::_drawSlots(const std::vector<Slot>& slots) {
    _drawSlotsOverlay(slots);
}

void GameView::_drawBlackRelay(const std::vector<sf::Vector2f>& path) {
    if (path.empty()) return;
    sf::Vector2f pos = path.back();

    for (int r = 30; r >= 8; r -= 8) {
        sf::CircleShape c((float)r);
        c.setOrigin({(float)r, (float)r});
        c.setPosition(pos);
        c.setFillColor(sf::Color::Transparent);
        c.setOutlineColor(sf::Color(200, 30, 30, 60 + (30-r)*4));
        c.setOutlineThickness(1.f);
        _window.draw(c);
    }
    sf::CircleShape core(8.f);
    core.setOrigin({8.f, 8.f});
    core.setPosition(pos);
    core.setFillColor(sf::Color(200, 30, 30, 220));
    _window.draw(core);

    sf::RectangleShape h({16.f, 2.f});
    h.setOrigin({8.f, 1.f}); h.setPosition(pos);
    h.setFillColor(sf::Color(255, 80, 80));
    _window.draw(h);
    sf::RectangleShape v({2.f, 16.f});
    v.setOrigin({1.f, 8.f}); v.setPosition(pos);
    v.setFillColor(sf::Color(255, 80, 80));
    _window.draw(v);

    sf::Text lbl(_font);
    lbl.setCharacterSize(11);
    lbl.setFillColor(sf::Color(200, 30, 30));
    lbl.setString("BLACK RELAY");
    lbl.setPosition({pos.x - 36.f, pos.y + 38.f});
    _window.draw(lbl);
}

void GameView::_drawHUD(Game& game) {
    _window.draw(_hudBar);
    sf::RectangleShape sep({800.f, 1.f});
    sep.setFillColor(sf::Color(200, 30, 30, 120));
    sep.setPosition({0.f, 50.f});
    _window.draw(sep);

    _window.draw(*_scoreText);
    _window.draw(*_resourceText);
    _window.draw(*_baseHpText);
    _window.draw(*_waveText);

    if (game.isWaveActive()) {
        sf::Text alert(_font);
        alert.setCharacterSize(11);
        alert.setFillColor(sf::Color(200, 30, 30));
        alert.setString("// VAGUE EN COURS");
        alert.setPosition({650.f, 16.f});
        _window.draw(alert);
    }

    sf::RectangleShape botBar({800.f, 22.f});
    botBar.setFillColor(sf::Color(10, 10, 14, 210));
    botBar.setPosition({0.f, 578.f});
    _window.draw(botBar);

    bool farm = game.isFarmPhase();
    if (farm) {
        sf::Text ft(_font);
        ft.setCharacterSize(11);
        ft.setFillColor(sf::Color(80, 220, 80));
        ft.setString("// PHASE FARM – Deploiement gratuit (vagues 1-3)");
        ft.setPosition({10.f, 560.f});
        _window.draw(ft);
    }

    const std::string& sel = game.getSelectedTower();
    auto drawKey = [&](const std::string& key, const std::string& id, float x) {
        std::string name = id; int cost = 0;
        if (TowerFactory::hasId(id)) {
            auto it = TowerFactory::getCatalog().find(id);
            if (it != TowerFactory::getCatalog().end()) {
                name = it->second.name; cost = it->second.shards;
            }
        }
        bool selected = (sel == id || sel == key);
        sf::Text t(_font); t.setCharacterSize(11);
        t.setFillColor(selected ? sf::Color(220, 220, 80) : sf::Color(110, 110, 110));
        std::string costStr = farm ? "FREE" : std::to_string(cost);
        t.setString("[" + key + "] " + name + " (" + costStr + ")");
        t.setPosition({x, 581.f}); _window.draw(t);
    };
    drawKey("1", "sniper",        10.f);
    drawKey("2", "heavy_gunner", 175.f);
    drawKey("3", "elementalist", 355.f);

    sf::Text misc(_font); misc.setCharacterSize(11);
    misc.setFillColor(sf::Color(70, 70, 70));
    misc.setString("[ESPACE] Vague  [P] Pause  [ESC] Menu");
    misc.setPosition({545.f, 581.f}); _window.draw(misc);
}

void GameView::_drawPause() {
    sf::RectangleShape overlay({800.f, 600.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    _window.draw(overlay);
    sf::RectangleShape panel({320.f, 200.f});
    panel.setFillColor(sf::Color(10, 10, 14, 235));
    panel.setOutlineColor(sf::Color(200, 30, 30, 160));
    panel.setOutlineThickness(1.f);
    panel.setPosition({240.f, 200.f}); _window.draw(panel);
    sf::RectangleShape tl({320.f, 2.f});
    tl.setFillColor(sf::Color(200, 30, 30));
    tl.setPosition({240.f, 200.f}); _window.draw(tl);
    sf::Text t(_font); t.setCharacterSize(28);
    t.setFillColor(sf::Color::White);
    t.setString("// PAUSE"); t.setPosition({290.f, 218.f}); _window.draw(t);
    sf::Text sub(_font); sub.setCharacterSize(16);
    sub.setFillColor(sf::Color(160, 160, 160));
    sub.setString("[P]    Reprendre\n[ESC]  Menu principal");
    sub.setPosition({270.f, 278.f}); _window.draw(sub);
}

void GameView::_drawEndScreen(bool victory) {
    sf::RectangleShape overlay({800.f, 600.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    _window.draw(overlay);
    sf::RectangleShape panel({420.f, 240.f});
    panel.setFillColor(sf::Color(10, 10, 14, 240));
    panel.setOutlineColor(victory ? sf::Color(80, 220, 80, 180) : sf::Color(200, 30, 30, 180));
    panel.setOutlineThickness(2.f);
    panel.setPosition({190.f, 170.f}); _window.draw(panel);
    sf::RectangleShape tl({420.f, 2.f});
    tl.setFillColor(victory ? sf::Color(80, 220, 80) : sf::Color(200, 30, 30));
    tl.setPosition({190.f, 170.f}); _window.draw(tl);
    sf::Text t(_font); t.setCharacterSize(38);
    t.setFillColor(victory ? sf::Color(80, 220, 80) : sf::Color(220, 60, 60));
    t.setString(victory ? "// VICTOIRE" : "// DEFAITE");
    t.setPosition({210.f, 188.f}); _window.draw(t);
    sf::Text sub(_font); sub.setCharacterSize(16);
    sub.setFillColor(sf::Color(160, 160, 160));
    sub.setString(victory ? "Le Black Relay est securise, Commandant."
                          : "Le Black Relay est tombe. Recommencer ?");
    sub.setPosition({210.f, 260.f}); _window.draw(sub);
    sf::Text action(_font); action.setCharacterSize(14);
    action.setFillColor(sf::Color(80, 80, 80));
    action.setString("[ENTREE] Rejouer          [ESC] Menu");
    action.setPosition({230.f, 360.f}); _window.draw(action);
}
