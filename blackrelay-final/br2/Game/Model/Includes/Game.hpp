#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Observer.hpp"
#include "../Enemy/Includes/Ennemy.hpp"
#include "../Player_Tower/Includes/Player_Tower.hpp"

enum class GameState { MENU, PLAYING, PAUSED, VICTORY, DEFEAT };

// ── Emplacement déployable ────────────────────────────────────────────────────
struct Slot {
    sf::Vector2f pos;
    bool         occupied = false;
};

// ── WaveManager ───────────────────────────────────────────────────────────────
class WaveManager {
public:
    WaveManager() : _currentWave(0), _spawnTimer(0.f),
                    _spawnInterval(1.2f), _spawnIndex(0), _waveActive(false) {}

    void startNextWave() {
        ++_currentWave;
        _queue.clear();
        _spawnIndex = 0;
        _waveActive = true;

        if (_currentWave <= 3) {
            int scouts = 3 + _currentWave * 2;
            for (int i = 0; i < scouts; ++i) _queue.push_back("scout");
        } else if (_currentWave == 4) {
            for (int i = 0; i < 6; ++i) _queue.push_back("scout");
            for (int i = 0; i < 3; ++i) _queue.push_back("infected");
        } else if (_currentWave == 5) {
            for (int i = 0; i < 6; ++i) _queue.push_back("scout");
            for (int i = 0; i < 4; ++i) _queue.push_back("infected");
            for (int i = 0; i < 2; ++i) _queue.push_back("destroyer");
        } else if (_currentWave == 6) {
            for (int i = 0; i < 5; ++i) _queue.push_back("scout");
            for (int i = 0; i < 4; ++i) _queue.push_back("infected");
            for (int i = 0; i < 3; ++i) _queue.push_back("destroyer");
            for (int i = 0; i < 2; ++i) _queue.push_back("blighter");
        } else if (_currentWave == 7) {
            for (int i = 0; i < 4; ++i) _queue.push_back("scout");
            for (int i = 0; i < 4; ++i) _queue.push_back("infected");
            for (int i = 0; i < 3; ++i) _queue.push_back("destroyer");
            for (int i = 0; i < 3; ++i) _queue.push_back("blighter");
            for (int i = 0; i < 2; ++i) _queue.push_back("overseer");
        } else if (_currentWave <= 9) {
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

    void update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies,
                const std::vector<sf::Vector2f>& path)
    {
        if (!_waveActive) return;
        _spawnTimer -= dt;
        if (_spawnTimer > 0.f) return;
        if (_spawnIndex < (int)_queue.size()) {
            auto e = EnemyFactory::create(_queue[_spawnIndex], _currentWave);
            if (!path.empty()) e->setPosition(path[0]);
            enemies.push_back(std::move(e));
            ++_spawnIndex;
            _spawnTimer = _spawnInterval;
        } else {
            _waveActive = false;
        }
    }

    bool isWaveActive() const { return _waveActive; }
    bool isWaveCleared(const std::vector<std::unique_ptr<Enemy>>& e) const {
        if (_waveActive) return false;
        for (auto& en : e) if (en->isAlive()) return false;
        return true;
    }
    int  getCurrentWave() const { return _currentWave; }
    int  getTotalWaves()  const { return 10; }
    bool isFarmPhase()    const { return _currentWave <= 3; }

private:
    int   _currentWave, _spawnIndex;
    float _spawnTimer, _spawnInterval;
    bool  _waveActive;
    std::vector<std::string> _queue;
};

// ── Game ──────────────────────────────────────────────────────────────────────
class Game : public IObservable {
public:
    Game();

    void update(float dt);
    bool placeTower(float x, float y);   // place sur le slot le plus proche
    void launchNextWave();
    void startGame();
    void setState(GameState s);
    void setSelectedTower(const std::string& type);

    GameState          getState()        const;
    int                getScore()        const;
    int                getResources()    const;
    int                getBaseHp()       const;
    int                getCurrentWave()  const;
    int                getTotalWaves()   const;
    bool               isWaveActive()    const;
    bool               isFarmPhase()     const;
    const std::string& getSelectedTower()const;

    std::vector<std::unique_ptr<Player_Tower>>& getTowers();
    std::vector<std::unique_ptr<Enemy>>&        getEnemies();
    const std::vector<sf::Vector2f>&            getPath()  const;
    const std::vector<Slot>&                    getSlots() const;

private:
    int         _score, _resources, _baseHp;
    GameState   _state;
    std::string _selectedTowerType;

    WaveManager                                _waveManager;
    std::vector<std::unique_ptr<Player_Tower>> _towers;
    std::vector<std::unique_ptr<Enemy>>        _enemies;

    // ── Map : chemin + emplacements ──
    std::vector<sf::Vector2f> _path;   // waypoints ennemis
    std::vector<Slot>         _slots;  // emplacements déployables

    void _initMap();
    Slot* _findNearestFreeSlot(float x, float y, float maxDist = 40.f);
};
