#include "../Includes/Game.hpp"
#include <algorithm>
#include <cmath>

// ── Constructeur ──────────────────────────────────────────────────────────────
GameView::GameView(sf::RenderWindow& window) : _window(window) {
    _font.openFromFile("Assets/Fonts/font.ttf");

    _scoreText.emplace(_font);
    _resourceText.emplace(_font);
    _baseHpText.emplace(_font);
    _waveText.emplace(_font);
    _towerInfoText.emplace(_font);

    _hasMapBg = _mapTexture.loadFromFile("Assets/Backgrounds/map_bg.png");
    if (_hasMapBg) {
        _mapSprite.emplace(_mapTexture);
        _mapSprite->setScale({1.f, 1.f});
        _mapSprite->setPosition({0.f, 50.f});
    }

    _setupHUD();
}

// ── Observer ──────────────────────────────────────────────────────────────────
void GameView::onEvent(const std::string& event, int value) {
    if      (event == "SCORE")     _scoreText->setString("Score: "     + std::to_string(value));
    else if (event == "RESOURCES") _resourceText->setString("Shards: " + std::to_string(value));
    else if (event == "BASE_HP")   _baseHpText->setString("Relais: "   + std::to_string(value) + " HP");
    else if (event == "WAVE")      _waveText->setString("Vague: "      + std::to_string(value) + "/10");
}

// ── Rendu principal ───────────────────────────────────────────────────────────
void GameView::render(Game& game) {
    _window.clear(sf::Color(14, 14, 20));

    switch (game.getState()) {
        case GameState::PLAYING:
            _drawGame(game);
            break;
        case GameState::PAUSED:
            _drawGame(game);
            _drawPause(game);
            break;
        case GameState::VICTORY:
            _drawGame(game);           // fond du jeu visible derrière
            _drawEndScreen(game, true);
            break;
        case GameState::DEFEAT:
            _drawGame(game);           // fond du jeu visible derrière
            _drawEndScreen(game, false);
            break;
        default:
            break;
    }

    _window.display();
}

// ── HUD setup ─────────────────────────────────────────────────────────────────
void GameView::_setupHUD() {
    _hudBar.setSize({1448.f, 50.f});
    _hudBar.setFillColor(sf::Color(10, 10, 14, 220));

    auto init = [&](std::optional<sf::Text>& t, float x, const std::string& s) {
        t->setCharacterSize(16);
        t->setFillColor(sf::Color::White);
        t->setPosition({x, 14.f});
        t->setString(s);
    };
    init(_scoreText,    10.f,  "Score: 0");
    init(_resourceText, 250.f, "Shards: 200");
    init(_baseHpText,   500.f, "Relais: 20 HP");
    init(_waveText,     780.f, "Vague: 0/10");
}

// ── Dessin du jeu ─────────────────────────────────────────────────────────────
void GameView::_drawGame(Game& game) {
    if (_hasMapBg && _mapSprite) {
        _window.draw(*_mapSprite);
        sf::RectangleShape overlay({1448.f, 1036.f});
        overlay.setPosition({0.f, 50.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 60));
        _window.draw(overlay);
    } else {
        _drawGrid();
        _drawPath(game.getPath());
        _drawSlots(game.getSlots());
    }

    _drawSlotsOverlay(game.getSlots());
    _drawBlackRelay(game.getPath());

    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);
    sf::Vector2f mouse    = { (float)mousePos.x, (float)mousePos.y };

    for (auto& t : game.getTowers()) {
        t->draw(_window, false);
        sf::Vector2f pos = t->getPosition();

        float distMouse = std::hypot(mouse.x - pos.x, mouse.y - pos.y);
        if (distMouse < 30.f) {
            float range = t->getRange();
            sf::CircleShape rangeCircle(range);
            rangeCircle.setOrigin({range, range});
            rangeCircle.setPosition(pos);
            rangeCircle.setFillColor(sf::Color(50, 120, 220, 30));
            rangeCircle.setOutlineColor(sf::Color(80, 160, 255, 160));
            rangeCircle.setOutlineThickness(1.5f);
            _window.draw(rangeCircle);
        }

        sf::Text name(_font);
        name.setCharacterSize(10);
        name.setFillColor(sf::Color(220, 210, 80));
        name.setString(t->getName() + " " + std::string(t->getLevel(), '*'));
        name.setPosition({pos.x - 20.f, pos.y - 32.f});
        _window.draw(name);

        float ratio = std::max(0.f, std::min(1.f, (float)t->getHp() / (float)t->getMaxHp()));
        float barW  = 40.f, barY = pos.y - 22.f;
        sf::RectangleShape hpBg({barW, 5.f});
        hpBg.setFillColor(sf::Color(60, 0, 0));
        hpBg.setPosition({pos.x - barW / 2.f, barY});
        _window.draw(hpBg);
        sf::Color barColor = ratio > 0.6f ? sf::Color(0, 210, 0)
                           : ratio > 0.3f ? sf::Color(220, 180, 0)
                                          : sf::Color(220, 50, 50);
        sf::RectangleShape hpBar({barW * ratio, 5.f});
        hpBar.setFillColor(barColor);
        hpBar.setPosition({pos.x - barW / 2.f, barY});
        _window.draw(hpBar);
    }

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

    _drawWaveTimer(game);
    _drawTowerMenu(game);
    _drawHUD(game);
}

// ── Timer entre vagues ────────────────────────────────────────────────────────
void GameView::_drawWaveTimer(Game& game) {
    if (!game.isWaitingNextWave()) return;
    float t = game.getWaveTimer();
    sf::RectangleShape panel({340.f, 52.f});
    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineColor(sf::Color(200, 30, 30, 180));
    panel.setOutlineThickness(1.f);
    panel.setPosition({554.f, 58.f});
    _window.draw(panel);
    sf::Text txt(_font);
    txt.setCharacterSize(18);
    txt.setFillColor(sf::Color(220, 200, 80));
    txt.setString("Prochaine vague dans : " + std::to_string((int)t + 1) + "s");
    txt.setPosition({564.f, 70.f});
    _window.draw(txt);
}

// ── Menu ingame ───────────────────────────────────────────────────────────────
void GameView::_drawTowerMenu(Game& game) {
    sf::Vector2i mp = sf::Mouse::getPosition(_window);
    static bool  wasPressed = false;
    bool         isPressed  = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool         clicked    = isPressed && !wasPressed;
    wasPressed = isPressed;

    const float panelX = 10.f;
    const float panelY = 960.f;
    const float panelW = 1428.f;
    const float panelH = 68.f;

    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(8, 8, 12, 220));
    bg.setOutlineColor(sf::Color(200, 30, 30, 80));
    bg.setOutlineThickness(1.f);
    bg.setPosition({panelX, panelY});
    _window.draw(bg);

    sf::Text title(_font);
    title.setCharacterSize(11);
    title.setFillColor(sf::Color(200, 30, 30));
    title.setString("// DEPLOIEMENT");
    title.setPosition({20.f, panelY + 6.f});
    _window.draw(title);

    const std::string& sel  = game.getSelectedTower();
    bool               farm = game.isFarmPhase();

    struct TowerEntry { std::string key; std::string id; };
    std::vector<TowerEntry> towers = {
        {"1","sniper"}, {"2","heavy_gunner"}, {"3","elementalist"},
        {"4","vanguard"}, {"5","guardian"}, {"6","assault"},
    };

    float x = 20.f;
    for (auto& entry : towers) {
        std::string name = entry.id;
        int cost = 0;
        if (TowerFactory::hasId(entry.id)) {
            auto it = TowerFactory::getCatalog().find(entry.id);
            if (it != TowerFactory::getCatalog().end()) {
                name = it->second.name;
                cost = it->second.shards;
            }
        }

        bool selected = (sel == entry.id);
        float btnW = 200.f, btnY = panelY + 22.f;
        bool hovered = (mp.x >= x && mp.x <= x + btnW - 8.f &&
                        mp.y >= btnY && mp.y <= btnY + 38.f);
        if (hovered && clicked) game.setSelectedTower(entry.id);

        sf::RectangleShape btn({btnW - 8.f, 38.f});
        btn.setFillColor(selected ? sf::Color(200,30,30,60)
                       : hovered  ? sf::Color(60,60,100,160)
                                  : sf::Color(20,20,28,180));
        btn.setOutlineColor(selected ? sf::Color(200,30,30,200)
                           : hovered  ? sf::Color(100,100,200,180)
                                      : sf::Color(60,60,80,120));
        btn.setOutlineThickness(1.f);
        btn.setPosition({x, btnY});
        _window.draw(btn);

        sf::Text key(_font); key.setCharacterSize(11);
        key.setFillColor(sf::Color(200,30,30));
        key.setString("[" + entry.key + "]");
        key.setPosition({x + 4.f, btnY + 4.f});
        _window.draw(key);

        sf::Text nameText(_font); nameText.setCharacterSize(12);
        nameText.setFillColor(selected ? sf::Color(255,255,180) : sf::Color(180,180,180));
        nameText.setString(name);
        nameText.setPosition({x + 30.f, btnY + 4.f});
        _window.draw(nameText);

        sf::Text costText(_font); costText.setCharacterSize(10);
        std::string costStr = farm ? "FREE" : std::to_string(cost) + " shards";
        costText.setFillColor(farm ? sf::Color(80,220,80) : sf::Color(140,140,140));
        costText.setString(costStr);
        costText.setPosition({x + 30.f, btnY + 22.f});
        _window.draw(costText);
        x += btnW;
    }

    // ── Bouton ABANDONNER ─────────────────────────────────────────────────────
    float abandonX = 1250.f, abandonY = panelY + 10.f;
    bool hoverAbandon = (mp.x >= abandonX && mp.x <= abandonX + 160.f &&
                         mp.y >= abandonY && mp.y <= abandonY + 46.f);
    if (hoverAbandon && clicked)
        game.setState(GameState::DEFEAT);  // abandon = défaite + score sauvegardé

    sf::RectangleShape abandonBtn({160.f, 46.f});
    abandonBtn.setFillColor(hoverAbandon ? sf::Color(180, 20, 20, 200)
                                         : sf::Color(80, 10, 10, 180));
    abandonBtn.setOutlineColor(sf::Color(220, 30, 30, hoverAbandon ? 255 : 120));
    abandonBtn.setOutlineThickness(1.f);
    abandonBtn.setPosition({abandonX, abandonY});
    _window.draw(abandonBtn);

    sf::Text abandonTxt(_font);
    abandonTxt.setCharacterSize(13);
    abandonTxt.setFillColor(sf::Color(220, 80, 80));
    abandonTxt.setString("[ ABANDONNER ]");
    abandonTxt.setPosition({abandonX + 8.f, abandonY + 14.f});
    _window.draw(abandonTxt);

    // Indicateur upgrade + farm
    sf::Text upgrade(_font); upgrade.setCharacterSize(11);
    upgrade.setFillColor(sf::Color(80, 160, 220));
    upgrade.setString("[CLIC DROIT] Ameliorer");
    upgrade.setPosition({1250.f, panelY + 58.f});
    _window.draw(upgrade);

    if (farm) {
        int count = game.getTowerCount();
        sf::Text farmText(_font); farmText.setCharacterSize(12);
        farmText.setFillColor(count >= 3 ? sf::Color(220,80,80) : sf::Color(80,220,80));
        farmText.setString("PHASE FARM  " + std::to_string(count) + "/3");
        farmText.setPosition({1050.f, panelY + 8.f});
        _window.draw(farmText);
    }
}

// ── Grille fallback ───────────────────────────────────────────────────────────
void GameView::_drawGrid() {
    for (int x = 0; x <= 1448; x += 40) {
        sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f((float)x, 50.f),   sf::Color(255,255,255,10)},
            sf::Vertex{sf::Vector2f((float)x, 1086.f), sf::Color(255,255,255,10)}
        };
        _window.draw(line, 2, sf::PrimitiveType::Lines);
    }
    for (int y = 50; y <= 1086; y += 40) {
        sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f(0.f,    (float)y), sf::Color(255,255,255,10)},
            sf::Vertex{sf::Vector2f(1448.f, (float)y), sf::Color(255,255,255,10)}
        };
        _window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

void GameView::_drawPath(const std::vector<sf::Vector2f>& path) {
    if (path.size() < 2) return;
    const float hw = 18.f;
    for (int i = 0; i + 1 < (int)path.size(); ++i) {
        sf::Vector2f a = path[i], b = path[i + 1];
        if (std::hypot(b.x-a.x, b.y-a.y) < 1.f) continue;
        if (std::abs(b.x-a.x) > std::abs(b.y-a.y)) {
            float x0 = std::min(a.x,b.x), x1 = std::max(a.x,b.x);
            sf::RectangleShape bg({x1-x0, hw*2});
            bg.setPosition({x0, a.y-hw});
            bg.setFillColor(sf::Color(40,10,10,180));
            _window.draw(bg);
        } else {
            float y0 = std::min(a.y,b.y), y1 = std::max(a.y,b.y);
            sf::RectangleShape bg({hw*2, y1-y0});
            bg.setPosition({a.x-hw, y0});
            bg.setFillColor(sf::Color(40,10,10,180));
            _window.draw(bg);
        }
    }
}

void GameView::_drawSlotsOverlay(const std::vector<Slot>& slots) {
    for (const auto& s : slots) {
        if (s.occupied) continue;
        sf::Color col(160, 160, 200, 140);
        sf::RectangleShape h({18.f, 2.f});
        h.setOrigin({9.f, 1.f}); h.setPosition(s.pos); h.setFillColor(col);
        _window.draw(h);
        sf::RectangleShape v({2.f, 18.f});
        v.setOrigin({1.f, 9.f}); v.setPosition(s.pos); v.setFillColor(col);
        _window.draw(v);
        sf::RectangleShape frame({30.f, 30.f});
        frame.setOrigin({15.f, 15.f}); frame.setPosition(s.pos);
        frame.setFillColor(sf::Color::Transparent);
        frame.setOutlineColor(sf::Color(140, 140, 180, 80));
        frame.setOutlineThickness(0.8f);
        _window.draw(frame);
    }
}

void GameView::_drawSlots(const std::vector<Slot>& slots) { _drawSlotsOverlay(slots); }

void GameView::_drawBlackRelay(const std::vector<sf::Vector2f>& path) {
    if (path.empty()) return;
    sf::Vector2f pos = path.back();
    for (int r = 30; r >= 8; r -= 8) {
        sf::CircleShape c((float)r);
        c.setOrigin({(float)r,(float)r}); c.setPosition(pos);
        c.setFillColor(sf::Color::Transparent);
        c.setOutlineColor(sf::Color(200,30,30,60+(30-r)*4));
        c.setOutlineThickness(1.f); _window.draw(c);
    }
    sf::CircleShape core(8.f);
    core.setOrigin({8.f,8.f}); core.setPosition(pos);
    core.setFillColor(sf::Color(200,30,30,220)); _window.draw(core);
    sf::RectangleShape h({16.f,2.f}); h.setOrigin({8.f,1.f}); h.setPosition(pos);
    h.setFillColor(sf::Color(255,80,80)); _window.draw(h);
    sf::RectangleShape v({2.f,16.f}); v.setOrigin({1.f,8.f}); v.setPosition(pos);
    v.setFillColor(sf::Color(255,80,80)); _window.draw(v);
    sf::Text lbl(_font); lbl.setCharacterSize(12);
    lbl.setFillColor(sf::Color(200,30,30)); lbl.setString("BLACK RELAY");
    lbl.setPosition({pos.x-40.f, pos.y+40.f}); _window.draw(lbl);
}

// ── HUD ───────────────────────────────────────────────────────────────────────
void GameView::_drawHUD(Game& game) {
    _window.draw(_hudBar);
    sf::RectangleShape sep({1448.f, 1.f});
    sep.setFillColor(sf::Color(200,30,30,120));
    sep.setPosition({0.f, 50.f}); _window.draw(sep);
    _window.draw(*_scoreText);
    _window.draw(*_resourceText);
    _window.draw(*_baseHpText);
    _window.draw(*_waveText);
    if (game.isWaveActive()) {
        sf::Text alert(_font); alert.setCharacterSize(12);
        alert.setFillColor(sf::Color(200,30,30));
        alert.setString("// VAGUE EN COURS");
        alert.setPosition({1050.f, 16.f}); _window.draw(alert);
    }
    sf::Text sound(_font); sound.setCharacterSize(11);
    sound.setFillColor(game.isSoundEnabled() ? sf::Color(80,200,80) : sf::Color(150,150,150));
    sound.setString(game.isSoundEnabled() ? "SON: ON [M]" : "SON: OFF [M]");
    sound.setPosition({1350.f, 16.f}); _window.draw(sound);
}

// ── Pause — avec bouton Abandonner ───────────────────────────────────────────
void GameView::_drawPause(Game& game) {
    sf::RectangleShape overlay({1448.f, 1086.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    _window.draw(overlay);

    sf::RectangleShape panel({360.f, 260.f});
    panel.setFillColor(sf::Color(10,10,14,235));
    panel.setOutlineColor(sf::Color(200,30,30,160));
    panel.setOutlineThickness(1.f);
    panel.setPosition({544.f, 413.f});
    _window.draw(panel);
    sf::RectangleShape tl({360.f, 2.f});
    tl.setFillColor(sf::Color(200,30,30));
    tl.setPosition({544.f, 413.f}); _window.draw(tl);

    sf::Text t(_font); t.setCharacterSize(30);
    t.setFillColor(sf::Color::White);
    t.setString("// PAUSE");
    t.setPosition({594.f, 433.f}); _window.draw(t);

    sf::Text sub(_font); sub.setCharacterSize(16);
    sub.setFillColor(sf::Color(160,160,160));
    sub.setString("[P]    Reprendre\n[ESC]  Menu principal\n\n[A]    Abandonner la partie");
    sub.setPosition({574.f, 493.f}); _window.draw(sub);
}

// ── Écran de fin — avec score affiché ────────────────────────────────────────
void GameView::_drawEndScreen(Game& game, bool victory) {
    sf::RectangleShape overlay({1448.f, 1086.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    _window.draw(overlay);

    sf::RectangleShape panel({520.f, 320.f});
    panel.setFillColor(sf::Color(10,10,14,240));
    panel.setOutlineColor(victory ? sf::Color(80,220,80,180) : sf::Color(200,30,30,180));
    panel.setOutlineThickness(2.f);
    panel.setPosition({464.f, 363.f});
    _window.draw(panel);

    sf::RectangleShape tl({520.f, 2.f});
    tl.setFillColor(victory ? sf::Color(80,220,80) : sf::Color(200,30,30));
    tl.setPosition({464.f, 363.f}); _window.draw(tl);

    // Titre
    sf::Text t(_font); t.setCharacterSize(42);
    t.setFillColor(victory ? sf::Color(80,220,80) : sf::Color(220,60,60));
    t.setString(victory ? "// VICTOIRE" : "// DEFAITE");
    t.setPosition({494.f, 380.f}); _window.draw(t);

    // Message
    sf::Text sub(_font); sub.setCharacterSize(15);
    sub.setFillColor(sf::Color(160,160,160));
    sub.setString(victory
        ? "Le Black Relay est securise. Bien joue, Commandant."
        : "Le Black Relay est tombe. Mission echouee.");
    sub.setPosition({494.f, 448.f}); _window.draw(sub);

    // Score final mis en valeur
    sf::RectangleShape scoreBg({300.f, 50.f});
    scoreBg.setFillColor(sf::Color(0, 0, 0, 160));
    scoreBg.setOutlineColor(victory ? sf::Color(80,220,80,100) : sf::Color(200,30,30,100));
    scoreBg.setOutlineThickness(1.f);
    scoreBg.setPosition({574.f, 490.f});
    _window.draw(scoreBg);

    sf::Text scoreLabel(_font); scoreLabel.setCharacterSize(13);
    scoreLabel.setFillColor(sf::Color(140,140,140));
    scoreLabel.setString("SCORE FINAL");
    scoreLabel.setPosition({690.f, 496.f}); _window.draw(scoreLabel);

    sf::Text scoreVal(_font); scoreVal.setCharacterSize(28);
    scoreVal.setFillColor(victory ? sf::Color(80,220,80) : sf::Color(220,100,100));
    scoreVal.setString(std::to_string(game.getScore()) + " pts");
    scoreVal.setPosition({660.f, 514.f}); _window.draw(scoreVal);

    // Vague atteinte
    sf::Text waveReached(_font); waveReached.setCharacterSize(14);
    waveReached.setFillColor(sf::Color(120,120,120));
    waveReached.setString("Vague atteinte : " + std::to_string(game.getCurrentWave())
                          + " / " + std::to_string(game.getTotalWaves()));
    waveReached.setPosition({574.f, 556.f}); _window.draw(waveReached);

    // Actions
    sf::Text action(_font); action.setCharacterSize(14);
    action.setFillColor(sf::Color(80,80,80));
    action.setString("[ENTREE]  Retour au menu          [ESC]  Quitter");
    action.setPosition({494.f, 640.f}); _window.draw(action);
}