#pragma once
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Observer.hpp"
#include "../Enemy/Includes/Ennemy.hpp"
#include "../Player_Tower/Includes/Player_Tower.hpp"

// ── États du jeu ──────────────────────────────────────────────────────────────
enum class GameState { MENU, PLAYING, PAUSED, VICTORY, DEFEAT };

// ── WaveManager : gère les vagues d'ennemis ───────────────────────────────────
class WaveManager {
public:
    WaveManager()
        : _currentWave(0), _spawnTimer(0.f), _spawnInterval(1.2f),
          _spawnIndex(0), _waveActive(false),
          _betweenWaveTimer(0.f), _betweenWaveDelay(10.f), _waitingNextWave(false) {}

    // Démarre la prochaine vague
    void startNextWave() {
        ++_currentWave;
        _queue.clear();
        _spawnIndex     = 0;
        _waveActive     = true;
        _waitingNextWave = false;

        // Composition des vagues selon le GDD
        if (_currentWave <= 3) {
            // Phase farm : scouts uniquement
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
            // Vague 10 : boss final + escorte
            for (int i = 0; i < 4; ++i) _queue.push_back("scout");
            for (int i = 0; i < 3; ++i) _queue.push_back("infected");
            for (int i = 0; i < 2; ++i) _queue.push_back("destroyer");
            _queue.push_back("boss");
        }
    }

    // Met à jour le spawn et le timer entre vagues
    bool update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies,
                const std::vector<sf::Vector2f>& path)
    {
        // Timer entre vagues : décompte automatique
        if (_waitingNextWave && _currentWave < getTotalWaves()) {
            _betweenWaveTimer -= dt;
            if (_betweenWaveTimer <= 0.f) {
                startNextWave();
                return true; // signal : nouvelle vague démarrée
            }
            return false;
        }

        if (!_waveActive) return false;

        // Spawn des ennemis un par un
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

    // Déclenche le timer d'attente avant la prochaine vague
    void triggerNextWaveTimer() {
        _waitingNextWave  = true;
        _betweenWaveTimer = _betweenWaveDelay;
    }

    bool  isWaveActive()    const { return _waveActive; }
    bool  isWaitingNextWave()const { return _waitingNextWave; }
    float getTimerRemaining()const { return _betweenWaveTimer; }

    bool isWaveCleared(const std::vector<std::unique_ptr<Enemy>>& enemies) const {
        if (_waveActive) return false;
        for (auto& e : enemies) if (e->isAlive()) return false;
        return true;
    }

    int  getCurrentWave() const { return _currentWave; }
    int  getTotalWaves()  const { return 10; }
    bool isFarmPhase()    const { return _currentWave <= 3; }

private:
    int   _currentWave, _spawnIndex;
    float _spawnTimer, _spawnInterval;
    bool  _waveActive;

    // Timer entre vagues
    float _betweenWaveTimer;
    float _betweenWaveDelay;  // secondes entre chaque vague
    bool  _waitingNextWave;

    std::vector<std::string> _queue;
};

// ── Slot : emplacement déployable sur la map ───────────────────────────────────
struct Slot {
    sf::Vector2f pos;
    bool         occupied = false;
};

// ── Game : modèle central MVC ─────────────────────────────────────────────────
class Game : public IObservable {
public:
    Game();

    // Mise à jour de la logique du jeu (appelée chaque frame)
    void update(float dt);

    // Place une tour sur le slot le plus proche du clic
    bool placeTower(float x, float y);

    // Upgrade la tour la plus proche du clic
    bool upgradeTower(float x, float y);

    // Lance la prochaine vague manuellement
    void launchNextWave();

    void startGame();
    void setState(GameState s);
    void setSelectedTower(const std::string& type);
    void setSoundEnabled(bool enabled);

    // Getters
    GameState          getState()         const;
    int                getScore()         const;
    int                getResources()     const;
    int                getBaseHp()        const;
    int                getCurrentWave()   const;
    int                getTotalWaves()    const;
    bool               isWaveActive()     const;
    bool               isFarmPhase()      const;
    bool               isWaitingNextWave()const;
    float              getWaveTimer()     const;
    int                getTowerCount()    const;
    bool               isSoundEnabled()   const;
    const std::string& getSelectedTower() const;

    std::vector<std::unique_ptr<Player_Tower>>& getTowers();
    std::vector<std::unique_ptr<Enemy>>&        getEnemies();
    const std::vector<sf::Vector2f>&            getPath()  const;
    const std::vector<Slot>&                    getSlots() const;

private:
    int         _score, _resources, _baseHp;
    GameState   _state;
    std::string _selectedTowerType;
    bool        _soundEnabled;

    WaveManager                                _waveManager;
    std::vector<std::unique_ptr<Player_Tower>> _towers;
    std::vector<std::unique_ptr<Enemy>>        _enemies;
    std::vector<sf::Vector2f>                  _path;
    std::vector<Slot>                          _slots;

    // Cooldowns d'attaque des ennemis sur les tours (pointeur ennemi → timer)
    std::map<Enemy*, float> _enemyAttackCooldowns;

    void  _initMap();
    Slot* _findNearestFreeSlot(float x, float y, float maxDist = 60.f);
};