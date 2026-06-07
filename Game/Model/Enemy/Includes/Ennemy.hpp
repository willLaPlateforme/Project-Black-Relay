#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <cmath>

struct EnemyData {
    std::string id;
    std::string name;
    int         pv         = 100;
    float       vitesse    = 2.f;
    int         dps        = 10;
    int         drop       = 10;
    float       scalingPv  = 0.15f;
    std::string typeDegats = "single";
};

enum class AnimState { IDLE, WALK, ATTACK, DEATH };

struct Animation {
    std::vector<sf::Texture> frames;
    float                    fps;
};

class Enemy {
public:
    Enemy(const EnemyData& data, int waveNumber);
    virtual ~Enemy() = default;

    virtual void move(const std::vector<sf::Vector2f>& path, float dt);
    virtual void takeDamage(int dmg);
    virtual void draw(sf::RenderWindow& window);

    void updateAnimation(float dt);

    void         kill();
    void         clearReward();
    bool         isAlive()       const;
    bool         isDead()        const;
    int          getReward()     const;
    int          getPathIndex()  const;
    int          getDps()        const;
    sf::Vector2f getPosition()   const;
    void         setPosition(sf::Vector2f pos);
    std::string  getId()         const;
    std::string  getTypeDegats() const;

protected:
    std::string _id;
    std::string _typeDegats;
    int         _hp, _maxHp;
    float       _speed;
    int         _reward;
    int         _pathIndex;
    int         _dps;
    bool        _alive;
    bool        _deathAnimDone;

    std::map<AnimState, Animation> _animations;
    AnimState                      _currentAnim;
    int                            _currentFrame;
    float                          _frameTimer;
    bool                           _spriteLoaded;
    std::optional<sf::Sprite>      _sprite;
    float                          _scale;

    sf::CircleShape    _shape;
    sf::RectangleShape _hpBarBg;
    sf::RectangleShape _hpBar;

    void _loadSprites();
    void _setAnim(AnimState s);
    void _drawHpBar(sf::RenderWindow& window);

    static sf::Color colorForId(const std::string& id);
    static float     radiusForId(const std::string& id);
    static float     scaleForId(const std::string& id);
};

class Scout : public Enemy {
public:
    Scout(const EnemyData& d, int w) : Enemy(d, w) {}
};

class Infected : public Enemy {
public:
    Infected(const EnemyData& d, int w) : Enemy(d, w) {}
};

class Destroyer : public Enemy {
public:
    Destroyer(const EnemyData& d, int w) : Enemy(d, w) {}
    void takeDamage(int dmg) override {
        Enemy::takeDamage((int)(dmg * 0.8f));
    }
};

class Blighter : public Enemy {
public:
    Blighter(const EnemyData& d, int w) : Enemy(d, w) {}
};

class Overseer : public Enemy {
public:
    Overseer(const EnemyData& d, int w) : Enemy(d, w) {}
};

class BossEnemy : public Enemy {
public:
    BossEnemy(const EnemyData& d, int w) : Enemy(d, w) {}
    void takeDamage(int dmg) override {
        Enemy::takeDamage((int)(dmg * 0.7f));
    }
};

// ── EnemyFactory ──────────────────────────────────────────────────────────────
class EnemyFactory {
public:
    static void loadFromJson(const std::string& path);
    static std::unique_ptr<Enemy> create(const std::string& id, int wave = 1);
    static void setDiffMult(float mult);   // <-- AJOUT difficulté
    static bool hasId(const std::string& id);
    static const std::map<std::string, EnemyData>& getCatalog();
    static inline float _diffMult = 1.0f;  // <-- AJOUT difficulté

private:
    static inline std::map<std::string, EnemyData> _catalog;
};