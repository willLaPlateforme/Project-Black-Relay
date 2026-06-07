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
#include "../../../Menu/Includes/Options.hpp"

enum class GameState { MENU, PLAYING, PAUSED, VICTORY, DEFEAT };

// ── WaveManager ───────────────────────────────────────────────────────────────
class WaveManager {
public:
    WaveManager()
        : _currentWave(0), _spawnTimer(0.f), _spawnInterval(1.2f),
          _spawnIndex(0), _waveActive(false),
          _betweenWaveTimer(0.f), _betweenWaveDelay(10.f),
          _waitingNextWave(false), _endless(false) {}

    void startNextWave();
    bool update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies,
                const std::vector<sf::Vector2f>& path);
    void triggerNextWaveTimer();
    void setEndless(bool e) { _endless = e; }

    bool  isWaveActive()     const { return _waveActive; }
    bool  isWaitingNextWave()const { return _waitingNextWave; }
    float getTimerRemaining()const { return _betweenWaveTimer; }
    bool  isWaveCleared(const std::vector<std::unique_ptr<Enemy>>& enemies) const;
    int   getCurrentWave()   const { return _currentWave; }
    int   getTotalWaves()    const { return _endless ? 9999 : 10; }
    bool  isFarmPhase()      const { return _currentWave <= 3; }
    bool  isEndless()        const { return _endless; }

private:
    int   _currentWave, _spawnIndex;
    float _spawnTimer, _spawnInterval;
    bool  _waveActive;
    float _betweenWaveTimer, _betweenWaveDelay;
    bool  _waitingNextWave;
    bool  _endless;
    std::vector<std::string> _queue;
};

struct Slot {
    sf::Vector2f pos;
    bool         occupied = false;
};

class Game : public IObservable {
public:
    Game();

    void update(float dt);
    bool placeTower(float x, float y);
    bool upgradeTower(float x, float y);
    void launchNextWave();
    void startGame();
    void setState(GameState s);
    void setSelectedTower(const std::string& type);
    void setSoundEnabled(bool enabled);
    void setDifficulte(Difficulte d);

    GameState          getState()          const;
    int                getScore()          const;
    int                getResources()      const;
    int                getBaseHp()         const;
    int                getCurrentWave()    const;
    int                getTotalWaves()     const;
    bool               isWaveActive()      const;
    bool               isFarmPhase()       const;
    bool               isWaitingNextWave() const;
    float              getWaveTimer()      const;
    int                getTowerCount()     const;
    bool               isSoundEnabled()    const;
    Difficulte         getDifficulte()     const;
    float              getDiffMult()       const;  // multiplicateur stats ennemis
    bool               isEndless()        const;
    const std::string& getSelectedTower()  const;

    std::vector<std::unique_ptr<Player_Tower>>& getTowers();
    std::vector<std::unique_ptr<Enemy>>&        getEnemies();
    const std::vector<sf::Vector2f>&            getPath()  const;
    const std::vector<Slot>&                    getSlots() const;

private:
    int         _score, _resources, _baseHp;
    GameState   _state;
    std::string _selectedTowerType;
    bool        _soundEnabled;
    Difficulte  _difficulte;
    float       _diffMult;   // 0.5 facile / 1.0 normal / 1.2 difficile / 1.0 endless
    bool        _endless;

    WaveManager                                _waveManager;
    std::vector<std::unique_ptr<Player_Tower>> _towers;
    std::vector<std::unique_ptr<Enemy>>        _enemies;
    std::vector<sf::Vector2f>                  _path;
    std::vector<Slot>                          _slots;
    std::map<Enemy*, float>                    _enemyAttackCooldowns;

    void  _initMap();
    Slot* _findNearestFreeSlot(float x, float y, float maxDist = 60.f);
};