#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <cmath>
#include <fstream>

class Enemy;

// ── TowerData : données lues depuis units.json ────────────────────────────────
struct TowerData {
    std::string id;
    std::string name;
    int         pv         = 100;
    int         shards     = 50;
    int         dps        = 20;
    std::string portee     = "moyenne";
    std::string typeDegats = "single";
    std::string upgrade1;
    std::string upgrade2;
    bool        unique     = false;
};

// ── Player_Tower ──────────────────────────────────────────────────────────────
class Player_Tower {
public:
    Player_Tower(const TowerData& data, float x, float y);
    virtual ~Player_Tower() = default;

    void attack(Enemy* target, std::vector<std::unique_ptr<Enemy>>& all);
    void update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies);
    void upgrade();
    void draw(sf::RenderWindow& window, bool showRange = false);
    void takeDamage(int dmg);
    int   getHp()    const;
    int   getMaxHp() const;
    float getRange()  const;

    sf::Vector2f       getPosition()    const;
    int                getLevel()       const;
    int                getCost()        const;
    const std::string& getId()          const;
    const std::string& getName()        const;
    const std::string& getUpgradeDesc() const;

protected:
    TowerData _data;
    int       _level;
    int       _hp;
    int       _maxHp;
    float     _range, _fireRate, _cooldown;
    int       _damage;

    sf::RectangleShape _shape;
    sf::CircleShape    _rangeCircle;

    Enemy* _findTarget(std::vector<std::unique_ptr<Enemy>>& enemies);

    static float     porteeToPixels(const std::string& p);
    static sf::Color colorForId(const std::string& id);
};

// ── TowerFactory ──────────────────────────────────────────────────────────────
class TowerFactory {
public:
    static void loadFromJson(const std::string& path);
    static std::unique_ptr<Player_Tower> create(const std::string& id, float x, float y);

    // Compatibilité touches 1/2/3 → vrais ids JSON
    static std::unique_ptr<Player_Tower> createCompat(const std::string& type,
                                                       float x, float y);
    static bool hasId(const std::string& id);
    static const std::map<std::string, TowerData>& getCatalog();

private:
    static inline std::map<std::string, TowerData> _catalog;
};
