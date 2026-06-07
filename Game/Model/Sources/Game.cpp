#include "../Includes/Game.hpp"

// ── WaveManager::startNextWave ────────────────────────────────────────────────
void WaveManager::startNextWave() {
    ++_currentWave;
    _queue.clear();
    _spawnIndex      = 0;
    _waveActive      = true;
    _waitingNextWave = false;

    // En mode endless, on recycle les compositions en boucle
    int wave = _endless ? ((_currentWave - 1) % 10) + 1 : _currentWave;

    if (wave <= 3) {
        int scouts = 3 + wave * 2;
        for (int i = 0; i < scouts; ++i) _queue.push_back("scout");
    } else if (wave == 4) {
        for (int i = 0; i < 6; ++i) _queue.push_back("scout");
        for (int i = 0; i < 3; ++i) _queue.push_back("infected");
    } else if (wave == 5) {
        for (int i = 0; i < 6; ++i) _queue.push_back("scout");
        for (int i = 0; i < 4; ++i) _queue.push_back("infected");
        for (int i = 0; i < 2; ++i) _queue.push_back("destroyer");
    } else if (wave == 6) {
        for (int i = 0; i < 5; ++i) _queue.push_back("scout");
        for (int i = 0; i < 4; ++i) _queue.push_back("infected");
        for (int i = 0; i < 3; ++i) _queue.push_back("destroyer");
        for (int i = 0; i < 2; ++i) _queue.push_back("blighter");
    } else if (wave == 7) {
        for (int i = 0; i < 4; ++i) _queue.push_back("scout");
        for (int i = 0; i < 4; ++i) _queue.push_back("infected");
        for (int i = 0; i < 3; ++i) _queue.push_back("destroyer");
        for (int i = 0; i < 3; ++i) _queue.push_back("blighter");
        for (int i = 0; i < 2; ++i) _queue.push_back("overseer");
    } else if (wave <= 9) {
        for (int i = 0; i < 4; ++i) _queue.push_back("scout");
        for (int i = 0; i < 4; ++i) _queue.push_back("infected");
        for (int i = 0; i < 4; ++i) _queue.push_back("destroyer");
        for (int i = 0; i < 3; ++i) _queue.push_back("blighter");
        for (int i = 0; i < 3; ++i) _queue.push_back("overseer");
    } else {
        for (int i = 0; i < 4; ++i) _queue.push_back("scout");
        for (int i = 0; i < 3; ++i) _queue.push_back("infected");
        for (int i = 0; i < 2; ++i) _queue.push_back("destroyer");
        _queue.push_back("boss");
    }
}

bool WaveManager::update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies,
                          const std::vector<sf::Vector2f>& path) {
    // En endless on continue toujours, sinon on s'arrête à getTotalWaves()
    bool canContinue = _endless || _currentWave < getTotalWaves();

    if (_waitingNextWave && canContinue) {
        _betweenWaveTimer -= dt;
        if (_betweenWaveTimer <= 0.f) {
            startNextWave();
            return true;
        }
        return false;
    }
    if (!_waveActive) return false;

    _spawnTimer -= dt;
    if (_spawnTimer > 0.f) return false;

    if (_spawnIndex < (int)_queue.size()) {
        auto e = EnemyFactory::create(_queue[_spawnIndex], _currentWave);
        if (!path.empty()) e->setPosition(path[0]);
        enemies.push_back(std::move(e));
        ++_spawnIndex;
        _spawnTimer = _spawnInterval;
    } else {
        _waveActive = false;
    }
    return false;
}

void WaveManager::triggerNextWaveTimer() {
    _waitingNextWave  = true;
    _betweenWaveTimer = _betweenWaveDelay;
}

bool WaveManager::isWaveCleared(const std::vector<std::unique_ptr<Enemy>>& enemies) const {
    if (_waveActive) return false;
    for (auto& e : enemies) if (e->isAlive()) return false;
    return true;
}

// ── Constructeur ──────────────────────────────────────────────────────────────
Game::Game()
    : _score(0), _resources(200), _baseHp(20),
      _state(GameState::MENU), _selectedTowerType("sniper"),
      _soundEnabled(true), _difficulte(Difficulte::NORMAL),
      _diffMult(1.0f), _endless(false)
{
    EnemyFactory::loadFromJson("Assets/Data/enemies.json");
    TowerFactory::loadFromJson("Assets/Data/units.json");
    _initMap();
}

void Game::_initMap() {
    _path = {
        { 1500.f, 290.f },
        { 1320.f, 290.f },
        {  890.f, 290.f },
        {  890.f, 530.f },
        {  650.f, 530.f },
        {  650.f, 730.f },
        {  405.f, 730.f },
        {  405.f, 840.f },
        {  200.f, 816.f },
    };

    _slots.push_back({ { 1180.f, 220.f }, false });
    _slots.push_back({ { 1080.f, 220.f }, false });
    _slots.push_back({ {  980.f, 220.f }, false });
    _slots.push_back({ {  880.f, 220.f }, false });
    _slots.push_back({ {  760.f, 220.f }, false });
    _slots.push_back({ {  770.f, 360.f }, false });
    _slots.push_back({ {  770.f, 440.f }, false });
    _slots.push_back({ {  650.f, 440.f }, false });
    _slots.push_back({ {  520.f, 440.f }, false });
    _slots.push_back({ {  510.f, 615.f }, false });
    _slots.push_back({ {  400.f, 615.f }, false });
    _slots.push_back({ {  230.f, 615.f }, false });
    _slots.push_back({ { 1085.f, 645.f }, false });
    _slots.push_back({ {  950.f, 645.f }, false });
    _slots.push_back({ {  810.f, 645.f }, false });
    _slots.push_back({ {  820.f, 865.f }, false });
    _slots.push_back({ {  680.f, 865.f }, false });
}

Slot* Game::_findNearestFreeSlot(float x, float y, float maxDist) {
    Slot* best     = nullptr;
    float bestDist = maxDist;
    for (auto& s : _slots) {
        if (s.occupied) continue;
        float d = std::hypot(s.pos.x - x, s.pos.y - y);
        if (d < bestDist) { bestDist = d; best = &s; }
    }
    return best;
}

// ── Update ────────────────────────────────────────────────────────────────────
void Game::update(float dt) {
    if (_state != GameState::PLAYING) return;

    for (auto& t : _towers) t->update(dt, _enemies);

    if (_waveManager.update(dt, _enemies, _path))
        notify("WAVE", _waveManager.getCurrentWave());

    for (auto& e : _enemies) {
        if (!e->isAlive()) continue;
        e->move(_path, dt);
        if (e->getPathIndex() >= (int)_path.size()) {
            e->kill();
            --_baseHp;
            notify("BASE_HP", _baseHp);
        }
    }

    for (auto& e : _enemies) {
        if (!e->isAlive()) continue;
        float& cd = _enemyAttackCooldowns[e.get()];
        cd -= dt;
        if (cd > 0.f) continue;
        const float attackRange = 150.f;
        Player_Tower* target = nullptr;
        float bestDist = attackRange;
        for (auto& t : _towers) {
            if (t->getHp() <= 0) continue;
            float d = std::hypot(t->getPosition().x - e->getPosition().x,
                                 t->getPosition().y - e->getPosition().y);
            if (d < bestDist) { bestDist = d; target = t.get(); }
        }
        if (target) { target->takeDamage(e->getDps()); cd = 0.8f; }
    }

    for (auto& t : _towers) {
        if (t->getHp() <= 0) {
            for (auto& s : _slots) {
                if (std::hypot(s.pos.x - t->getPosition().x,
                               s.pos.y - t->getPosition().y) < 5.f) {
                    s.occupied = false; break;
                }
            }
        }
    }
    _towers.erase(std::remove_if(_towers.begin(), _towers.end(),
        [](const std::unique_ptr<Player_Tower>& t){ return t->getHp() <= 0; }),
        _towers.end());

    for (auto& e : _enemies)
        if (!e->isAlive()) _enemyAttackCooldowns.erase(e.get());

    for (auto& e : _enemies) {
        if (!e->isAlive() && e->getReward() > 0) {
            _score     += e->getReward();
            _resources += e->getReward();
            e->clearReward();
            notify("SCORE",     _score);
            notify("RESOURCES", _resources);
        }
    }

    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(),
        [](const std::unique_ptr<Enemy>& e){ return !e->isAlive(); }),
        _enemies.end());

    if (_baseHp <= 0) { setState(GameState::DEFEAT); return; }

    if (_waveManager.isWaveCleared(_enemies) && !_waveManager.isWaitingNextWave()) {
        // Endless : jamais de victoire, on continue
        if (!_endless && _waveManager.getCurrentWave() >= _waveManager.getTotalWaves()) {
            setState(GameState::VICTORY);
        } else if (_waveManager.getCurrentWave() > 0) {
            _waveManager.triggerNextWaveTimer();
            notify("WAVE_CLEAR", _waveManager.getCurrentWave());
        }
    }
}

bool Game::placeTower(float x, float y) {
    Slot* slot = _findNearestFreeSlot(x, y, 60.f);
    if (!slot) return false;
    if (_waveManager.isFarmPhase() && (int)_towers.size() >= 3) return false;
    int cost = 0;
    if (!_waveManager.isFarmPhase()) {
        if (TowerFactory::hasId(_selectedTowerType)) {
            auto it = TowerFactory::getCatalog().find(_selectedTowerType);
            if (it != TowerFactory::getCatalog().end()) cost = it->second.shards;
        }
        if (_resources < cost) return false;
    }
    _towers.push_back(TowerFactory::createCompat(_selectedTowerType, slot->pos.x, slot->pos.y));
    slot->occupied = true;
    _resources    -= cost;
    notify("RESOURCES", _resources);
    notify("TOWERS",    (int)_towers.size());
    return true;
}

bool Game::upgradeTower(float x, float y) {
    Player_Tower* target = nullptr;
    float bestDist = 60.f;
    for (auto& t : _towers) {
        float d = std::hypot(t->getPosition().x - x, t->getPosition().y - y);
        if (d < bestDist) { bestDist = d; target = t.get(); }
    }
    if (!target) return false;
    int cost = target->getCost();
    if (_resources < cost) return false;
    target->upgrade();
    _resources -= cost;
    notify("RESOURCES", _resources);
    return true;
}

void Game::launchNextWave() {
    if (_state != GameState::PLAYING || _waveManager.isWaveActive()) return;
    _waveManager.startNextWave();
    notify("WAVE", _waveManager.getCurrentWave());
}

// ── startGame : applique le multiplicateur de difficulté ──────────────────────
void Game::startGame() {
    _score     = 0;
    _resources = 200;
    _baseHp    = 20;
    _towers.clear();
    _enemies.clear();
    _enemyAttackCooldowns.clear();
    for (auto& s : _slots) s.occupied = false;

    // Appliquer la difficulté
    switch (_difficulte) {
        case Difficulte::FACILE:
            _diffMult = 0.5f;
            _endless  = false;
            break;
        case Difficulte::NORMAL:
            _diffMult = 1.0f;
            _endless  = false;
            break;
        case Difficulte::DIFFICILE:
            _diffMult = 1.2f;
            _endless  = false;
            break;
        case Difficulte::ENDLESS:
            _diffMult = 1.0f;
            _endless  = true;
            break;
    }
    _waveManager.setEndless(_endless);

    // Transmettre le multiplicateur à EnemyFactory
    EnemyFactory::setDiffMult(_diffMult);

    setState(GameState::PLAYING);
    notify("SCORE",     0);
    notify("RESOURCES", 200);
    notify("BASE_HP",   20);
    notify("WAVE",      0);
}

void Game::setState(GameState s)                  { _state = s; notify("STATE", (int)s); }
void Game::setSelectedTower(const std::string& t) { _selectedTowerType = t; }
void Game::setSoundEnabled(bool enabled)          { _soundEnabled = enabled; }
void Game::setDifficulte(Difficulte d)            { _difficulte = d; }

GameState          Game::getState()          const { return _state; }
int                Game::getScore()          const { return _score; }
int                Game::getResources()      const { return _resources; }
int                Game::getBaseHp()         const { return _baseHp; }
int                Game::getCurrentWave()    const { return _waveManager.getCurrentWave(); }
int                Game::getTotalWaves()     const { return _waveManager.getTotalWaves(); }
bool               Game::isWaveActive()      const { return _waveManager.isWaveActive(); }
bool               Game::isFarmPhase()       const { return _waveManager.isFarmPhase(); }
bool               Game::isWaitingNextWave() const { return _waveManager.isWaitingNextWave(); }
float              Game::getWaveTimer()      const { return _waveManager.getTimerRemaining(); }
int                Game::getTowerCount()     const { return (int)_towers.size(); }
bool               Game::isSoundEnabled()    const { return _soundEnabled; }
Difficulte         Game::getDifficulte()     const { return _difficulte; }
float              Game::getDiffMult()       const { return _diffMult; }
bool               Game::isEndless()         const { return _endless; }
const std::string& Game::getSelectedTower()  const { return _selectedTowerType; }

std::vector<std::unique_ptr<Player_Tower>>& Game::getTowers()  { return _towers; }
std::vector<std::unique_ptr<Enemy>>&        Game::getEnemies() { return _enemies; }
const std::vector<sf::Vector2f>&            Game::getPath()    const { return _path; }
const std::vector<Slot>&                    Game::getSlots()   const { return _slots; }