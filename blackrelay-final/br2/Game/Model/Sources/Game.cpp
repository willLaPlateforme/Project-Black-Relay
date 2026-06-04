#include "../Includes/Game.hpp"

// ── Constructeur : charge les données JSON et initialise la map ───────────────
Game::Game()
    : _score(0), _resources(200), _baseHp(20),
      _state(GameState::MENU), _selectedTowerType("sniper"),
      _soundEnabled(true)
{
    EnemyFactory::loadFromJson("Assets/Data/enemies.json");
    TowerFactory::loadFromJson("Assets/Data/units.json");
    _initMap();
}

// ── Initialisation du chemin et des emplacements déployables ─────────────────
void Game::_initMap() {
    // Chemin des ennemis : série de waypoints en pixels (fenêtre 1448x1030)
    // Les ennemis suivent ce chemin du spawn (hors écran) jusqu'au Black Relay
    _path = {
        { 1500.f, 290.f },  // spawn hors écran (droite)
        { 1320.f, 290.f },  // entrée spawn visible
        {  890.f, 290.f },  // virage 1
        {  890.f, 530.f },  // virage 2
        {  650.f, 530.f },  // virage 3
        {  650.f, 730.f },  // virage 4
        {  405.f, 730.f },  // virage 5
        {  405.f, 840.f },  // virage 6
        {  200.f, 816.f },  // Black Relay (objectif à défendre)
    };

    // Emplacements déployables : positions calibrées sur la map image
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

// ── Recherche du slot libre le plus proche d'un point ────────────────────────
Slot* Game::_findNearestFreeSlot(float x, float y, float maxDist) {
    Slot* best    = nullptr;
    float bestDist = maxDist;
    for (auto& s : _slots) {
        if (s.occupied) continue;
        float d = std::hypot(s.pos.x - x, s.pos.y - y);
        if (d < bestDist) { bestDist = d; best = &s; }
    }
    return best;
}

// ── Mise à jour principale du jeu (appelée chaque frame) ─────────────────────
void Game::update(float dt) {
    if (_state != GameState::PLAYING) return;

    // 1. Les tours attaquent les ennemis dans leur portée
    for (auto& t : _towers) t->update(dt, _enemies);

    // 2. Le WaveManager spawn les ennemis et gère le timer entre vagues
    // Si le timer automatique a démarré une nouvelle vague, notifier la vue
    if (_waveManager.update(dt, _enemies, _path))
        notify("WAVE", _waveManager.getCurrentWave());

    // 3. Déplacement des ennemis le long du chemin
    for (auto& e : _enemies) {
        if (!e->isAlive()) continue;
        e->move(_path, dt);

        // Si l'ennemi atteint le Black Relay : perd un HP de base
        if (e->getPathIndex() >= (int)_path.size()) {
            e->kill();
            --_baseHp;
            notify("BASE_HP", _baseHp);
        }
    }

    // 4. Ennemis attaquent les tours à proximité EN PASSANT (sans s'arrêter)
    for (auto& e : _enemies) {
        if (!e->isAlive()) continue;

        float& cd = _enemyAttackCooldowns[e.get()];
        cd -= dt;
        if (cd > 0.f) continue;

        // Portée d'attaque élargie pour toucher les tours adjacentes au chemin
        const float attackRange = 150.f;
        Player_Tower* target    = nullptr;
        float         bestDist  = attackRange;

        for (auto& t : _towers) {
            if (t->getHp() <= 0) continue;
            float d = std::hypot(t->getPosition().x - e->getPosition().x,
                                 t->getPosition().y - e->getPosition().y);
            if (d < bestDist) { bestDist = d; target = t.get(); }
        }

        if (target) {
            target->takeDamage(e->getDps());
            cd = 0.8f; // une attaque toutes les 0.8 secondes
        }
    }

    // 5. Supprimer les tours détruites et libérer leur slot
    for (auto& t : _towers) {
        if (t->getHp() <= 0) {
            for (auto& s : _slots) {
                if (std::hypot(s.pos.x - t->getPosition().x,
                               s.pos.y - t->getPosition().y) < 5.f) {
                    s.occupied = false;
                    break;
                }
            }
        }
    }
    _towers.erase(std::remove_if(_towers.begin(), _towers.end(),
        [](const std::unique_ptr<Player_Tower>& t){ return t->getHp() <= 0; }),
        _towers.end());

    // 6. Nettoyer les cooldowns des ennemis morts
    for (auto& e : _enemies)
        if (!e->isAlive()) _enemyAttackCooldowns.erase(e.get());

    // 7. Collecter les récompenses des ennemis tués
    for (auto& e : _enemies) {
        if (!e->isAlive() && e->getReward() > 0) {
            _score     += e->getReward();
            _resources += e->getReward();
            e->clearReward();
            notify("SCORE",     _score);
            notify("RESOURCES", _resources);
        }
    }

    // 8. Supprimer les ennemis morts de la liste
    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(),
        [](const std::unique_ptr<Enemy>& e){ return !e->isAlive(); }),
        _enemies.end());

    // 9. Vérifier les conditions de fin de partie
    if (_baseHp <= 0) {
        setState(GameState::DEFEAT);
        return;
    }

    // Si la vague est terminée, déclencher le timer avant la suivante
    if (_waveManager.isWaveCleared(_enemies) && !_waveManager.isWaitingNextWave()) {
        if (_waveManager.getCurrentWave() >= _waveManager.getTotalWaves()) {
            setState(GameState::VICTORY);
        } else if (_waveManager.getCurrentWave() > 0) {
            _waveManager.triggerNextWaveTimer();
            notify("WAVE_CLEAR", _waveManager.getCurrentWave());
        }
    }
}

// ── Placement d'une tour sur le slot le plus proche ───────────────────────────
bool Game::placeTower(float x, float y) {
    Slot* slot = _findNearestFreeSlot(x, y, 60.f);
    if (!slot) return false;

    // Phase farm (vagues 1-3) : maximum 3 tours simultanées, déploiement gratuit
    if (_waveManager.isFarmPhase() && (int)_towers.size() >= 3) return false;

    // Vague 4+ : coût en Relay Shards
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
    _resources    -= cost;
    notify("RESOURCES", _resources);
    notify("TOWERS",    (int)_towers.size());
    return true;
}

// ── Amélioration d'une tour existante (clic droit) ────────────────────────────
bool Game::upgradeTower(float x, float y) {
    const float upgradeRange = 60.f;
    Player_Tower* target = nullptr;
    float bestDist = upgradeRange;

    // Trouve la tour la plus proche du clic
    for (auto& t : _towers) {
        float d = std::hypot(t->getPosition().x - x, t->getPosition().y - y);
        if (d < bestDist) { bestDist = d; target = t.get(); }
    }

    if (!target) return false;

    // Coût d'upgrade = coût de base de la tour
    int cost = target->getCost();
    if (_resources < cost) return false;

    target->upgrade();
    _resources -= cost;
    notify("RESOURCES", _resources);
    return true;
}

// ── Lancement manuel de la prochaine vague ────────────────────────────────────
void Game::launchNextWave() {
    if (_state != GameState::PLAYING || _waveManager.isWaveActive()) return;
    _waveManager.startNextWave();
    notify("WAVE", _waveManager.getCurrentWave());
}

// ── Réinitialisation et démarrage d'une nouvelle partie ──────────────────────
void Game::startGame() {
    _score     = 0;
    _resources = 200;
    _baseHp    = 20;
    _towers.clear();
    _enemies.clear();
    _enemyAttackCooldowns.clear();
    for (auto& s : _slots) s.occupied = false;

    setState(GameState::PLAYING);
    notify("SCORE",    0);
    notify("RESOURCES",200);
    notify("BASE_HP",  20);
    notify("WAVE",     0);
}

void Game::setState(GameState s)              { _state = s; notify("STATE", (int)s); }
void Game::setSelectedTower(const std::string& t) { _selectedTowerType = t; }
void Game::setSoundEnabled(bool enabled)      { _soundEnabled = enabled; }

GameState          Game::getState()         const { return _state; }
int                Game::getScore()         const { return _score; }
int                Game::getResources()     const { return _resources; }
int                Game::getBaseHp()        const { return _baseHp; }
int                Game::getCurrentWave()   const { return _waveManager.getCurrentWave(); }
int                Game::getTotalWaves()    const { return _waveManager.getTotalWaves(); }
bool               Game::isWaveActive()     const { return _waveManager.isWaveActive(); }
bool               Game::isFarmPhase()      const { return _waveManager.isFarmPhase(); }
bool               Game::isWaitingNextWave()const { return _waveManager.isWaitingNextWave(); }
float              Game::getWaveTimer()     const { return _waveManager.getTimerRemaining(); }
int                Game::getTowerCount()    const { return (int)_towers.size(); }
bool               Game::isSoundEnabled()   const { return _soundEnabled; }
const std::string& Game::getSelectedTower() const { return _selectedTowerType; }

std::vector<std::unique_ptr<Player_Tower>>& Game::getTowers()  { return _towers; }
std::vector<std::unique_ptr<Enemy>>&        Game::getEnemies() { return _enemies; }
const std::vector<sf::Vector2f>&            Game::getPath()    const { return _path; }
const std::vector<Slot>&                    Game::getSlots()   const { return _slots; }