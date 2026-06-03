#include "../Includes/Game.hpp"

Game::Game()
    : _score(0), _resources(200), _baseHp(20),
      _state(GameState::MENU), _selectedTowerType("sniper")
{
    EnemyFactory::loadFromJson("Assets/Data/enemies.json");
    TowerFactory::loadFromJson("Assets/Data/units.json");
    _initMap();
}

void Game::_initMap() {

    // ── CHEMIN calé précisément sur la maquette ───────────────────────────────
    // Fenêtre 800x600, HUD 50px haut
    // Spawn haut-droite → serpentin → Black Relay bas-gauche

    _path = {
        { 820.f, 160.f },   // spawn hors écran
        { 430.f, 160.f },   // segment 1 : droite→gauche (haut)
        { 430.f, 260.f },   // virage 1 : descend
        { 150.f, 260.f },   // segment 2 : droite→gauche (milieu haut)
        { 150.f, 360.f },   // virage 2 : descend
        { 490.f, 360.f },   // segment 3 : gauche→droite (milieu bas)
        { 490.f, 460.f },   // virage 3 : descend
        { 150.f, 460.f },   // segment 4 : droite→gauche (bas)
        { 150.f, 530.f },   // descend vers Black Relay
        {  85.f, 530.f },   // Black Relay
    };

    // ── SLOTS calés sur les croix de la maquette ─────────────────────────────
    // Rangée haute (au dessus segment 1, y≈105)
    _slots.push_back({ { 170.f, 105.f }, false });
    _slots.push_back({ { 295.f, 105.f }, false });
    _slots.push_back({ { 400.f, 110.f }, false });
    _slots.push_back({ { 505.f, 105.f }, false });
    _slots.push_back({ { 625.f, 105.f }, false });

    // Rangée milieu-haut (entre seg1 et seg2, y≈205)
    _slots.push_back({ { 270.f, 208.f }, false });
    _slots.push_back({ { 375.f, 208.f }, false });

    // Rangée milieu (entre seg2 et seg3, y≈310)
    _slots.push_back({ { 175.f, 310.f }, false });
    _slots.push_back({ { 295.f, 310.f }, false });
    _slots.push_back({ { 400.f, 310.f }, false });
    _slots.push_back({ { 520.f, 310.f }, false });
    _slots.push_back({ { 625.f, 310.f }, false });

    // Rangée bas (entre seg3 et seg4, y≈415)
    _slots.push_back({ { 295.f, 415.f }, false });
    _slots.push_back({ { 400.f, 415.f }, false });

    // Rangée très bas (sous seg4, y≈500)
    _slots.push_back({ { 295.f, 500.f }, false });
    _slots.push_back({ { 400.f, 500.f }, false });
}

Slot* Game::_findNearestFreeSlot(float x, float y, float maxDist) {
    Slot* best = nullptr;
    float bestDist = maxDist;
    for (auto& s : _slots) {
        if (s.occupied) continue;
        float dx = s.pos.x - x;
        float dy = s.pos.y - y;
        float d  = std::hypot(dx, dy);
        if (d < bestDist) { bestDist = d; best = &s; }
    }
    return best;
}

void Game::update(float dt) {
    if (_state != GameState::PLAYING) return;

    for (auto& t : _towers) t->update(dt, _enemies);
    _waveManager.update(dt, _enemies, _path);

    for (auto& e : _enemies) {
        if (!e->isAlive()) continue;
        e->move(_path, dt);
        if (e->getPathIndex() >= (int)_path.size()) {
            e->kill(); --_baseHp;
            notify("BASE_HP", _baseHp);
        }
    }

    for (auto& e : _enemies) {
        if (!e->isAlive() && e->getReward() > 0) {
            _score     += e->getReward();
            _resources += e->getReward();
            e->clearReward();
            notify("SCORE",     _score);
            notify("RESOURCES", _resources);
        }
    }

    _enemies.erase(
        std::remove_if(_enemies.begin(), _enemies.end(),
            [](const std::unique_ptr<Enemy>& e){ return !e->isAlive(); }),
        _enemies.end());

    if (_baseHp <= 0) { setState(GameState::DEFEAT); return; }
    if (_waveManager.isWaveCleared(_enemies) &&
        _waveManager.getCurrentWave() >= _waveManager.getTotalWaves())
        setState(GameState::VICTORY);
}

bool Game::placeTower(float x, float y) {
    Slot* slot = _findNearestFreeSlot(x, y, 55.f);
    if (!slot) return false;

    int cost = 0;
    if (!_waveManager.isFarmPhase()) {
        if (TowerFactory::hasId(_selectedTowerType)) {
            auto it = TowerFactory::getCatalog().find(_selectedTowerType);
            if (it != TowerFactory::getCatalog().end()) cost = it->second.shards;
        }
        if (_resources < cost) return false;
    }

    _towers.push_back(TowerFactory::createCompat(_selectedTowerType,
                                                  slot->pos.x, slot->pos.y));
    slot->occupied = true;
    _resources -= cost;
    notify("RESOURCES", _resources);
    return true;
}

void Game::launchNextWave() {
    if (_state != GameState::PLAYING || _waveManager.isWaveActive()) return;
    _waveManager.startNextWave();
    notify("WAVE", _waveManager.getCurrentWave());
}

void Game::startGame() {
    _score = 0; _resources = 200; _baseHp = 20;
    _towers.clear(); _enemies.clear();
    for (auto& s : _slots) s.occupied = false;
    setState(GameState::PLAYING);
    notify("SCORE", 0); notify("RESOURCES", 200);
    notify("BASE_HP", 20); notify("WAVE", 0);
}

void Game::setState(GameState s) { _state = s; notify("STATE", (int)s); }
void Game::setSelectedTower(const std::string& t) { _selectedTowerType = t; }

GameState          Game::getState()        const { return _state; }
int                Game::getScore()        const { return _score; }
int                Game::getResources()    const { return _resources; }
int                Game::getBaseHp()       const { return _baseHp; }
int                Game::getCurrentWave()  const { return _waveManager.getCurrentWave(); }
int                Game::getTotalWaves()   const { return _waveManager.getTotalWaves(); }
bool               Game::isWaveActive()    const { return _waveManager.isWaveActive(); }
bool               Game::isFarmPhase()     const { return _waveManager.isFarmPhase(); }
const std::string& Game::getSelectedTower()const { return _selectedTowerType; }

std::vector<std::unique_ptr<Player_Tower>>& Game::getTowers()  { return _towers; }
std::vector<std::unique_ptr<Enemy>>&        Game::getEnemies() { return _enemies; }
const std::vector<sf::Vector2f>&            Game::getPath()    const { return _path; }
const std::vector<Slot>&                    Game::getSlots()   const { return _slots; }