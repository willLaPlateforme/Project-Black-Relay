#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <cmath>

// ── EnemyData : données chargées depuis enemies.json ─────────────────────────
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

// ── AnimState : état d'animation possible ────────────────────────────────────
enum class AnimState { IDLE, WALK, ATTACK, DEATH };

// ── Animation : séquence de frames SFML ─────────────────────────────────────
struct Animation {
    std::vector<sf::Texture> frames;  // textures de chaque frame
    float                    fps;     // vitesse d'animation
};

// ── Enemy : classe de base avec système d'animation ─────────────────────────
class Enemy {
public:
    Enemy(const EnemyData& data, int waveNumber);
    virtual ~Enemy() = default;

    virtual void move(const std::vector<sf::Vector2f>& path, float dt);
    virtual void takeDamage(int dmg);
    virtual void draw(sf::RenderWindow& window);

    // Met à jour l'animation selon l'état (appelé chaque frame)
    void updateAnimation(float dt);

    void         kill();
    void         clearReward();
    bool         isAlive()       const;
    bool         isDead()        const; // mort ET animation de mort terminée
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

    // ── Système de sprite animé ───────────────────────────────────────────────
    std::map<AnimState, Animation> _animations;  // toutes les animations
    AnimState                      _currentAnim; // animation en cours
    int                            _currentFrame;// frame courante
    float                          _frameTimer;  // timer pour changer de frame
    bool                           _spriteLoaded;// sprites chargés avec succès
    std::optional<sf::Sprite>      _sprite;      // sprite actuel (SFML3 : nécessite une texture)
    float                          _scale;       // échelle d'affichage

    // Fallback : cercle coloré si pas de sprite
    sf::CircleShape    _shape;
    sf::RectangleShape _hpBarBg;
    sf::RectangleShape _hpBar;

    // ── Méthodes internes ─────────────────────────────────────────────────────
    void _loadSprites();        // charge les PNG depuis Assets/Sprites/{id}/
    void _setAnim(AnimState s); // change d'animation
    void _drawHpBar(sf::RenderWindow& window);

    static sf::Color colorForId(const std::string& id);
    static float     radiusForId(const std::string& id);
    static float     scaleForId(const std::string& id);
};

// ── Sous-classes : chaque ennemi avec ses particularités ─────────────────────

// Scout : rapide, peu résistant
class Scout : public Enemy {
public:
    Scout(const EnemyData& d, int w) : Enemy(d, w) {}
};

// Infected : unité standard équilibrée
class Infected : public Enemy {
public:
    Infected(const EnemyData& d, int w) : Enemy(d, w) {}
};

// Destroyer : très résistant, armure 20%
class Destroyer : public Enemy {
public:
    Destroyer(const EnemyData& d, int w) : Enemy(d, w) {}
    void takeDamage(int dmg) override {
        Enemy::takeDamage((int)(dmg * 0.8f)); // armure 20%
    }
};

// Blighter : dégâts AoE (gérés dans Game)
class Blighter : public Enemy {
public:
    Blighter(const EnemyData& d, int w) : Enemy(d, w) {}
};

// Overseer : soutien (boost alliés, géré dans Game)
class Overseer : public Enemy {
public:
    Overseer(const EnemyData& d, int w) : Enemy(d, w) {}
};

// Boss : armure 30%, très puissant
class BossEnemy : public Enemy {
public:
    BossEnemy(const EnemyData& d, int w) : Enemy(d, w) {}
    void takeDamage(int dmg) override {
        Enemy::takeDamage((int)(dmg * 0.7f)); // armure 30%
    }
};

// ── EnemyFactory : crée les ennemis depuis leur id ───────────────────────────
class EnemyFactory {
public:
    static void loadFromJson(const std::string& path);
    static std::unique_ptr<Enemy> create(const std::string& id, int wave = 1);
    static bool hasId(const std::string& id);
    static const std::map<std::string, EnemyData>& getCatalog();

private:
    static inline std::map<std::string, EnemyData> _catalog;
};
