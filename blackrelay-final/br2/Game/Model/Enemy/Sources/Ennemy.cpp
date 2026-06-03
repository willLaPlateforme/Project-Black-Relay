#include "../Includes/Ennemy.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

// ── Couleurs et rayons par id ─────────────────────────────────────────────────
sf::Color Enemy::colorForId(const std::string& id) {
    if (id == "scout")     return sf::Color(80,  200, 80);
    if (id == "infected")  return sf::Color(180, 120, 60);
    if (id == "destroyer") return sf::Color(100, 100, 210);
    if (id == "blighter")  return sf::Color(210, 100, 200);
    if (id == "overseer")  return sf::Color(60,  180, 220);
    if (id == "boss")      return sf::Color(220, 40,  40);
    return sf::Color(160, 160, 160);
}

float Enemy::radiusForId(const std::string& id) {
    if (id == "scout")     return 12.f;
    if (id == "infected")  return 15.f;
    if (id == "destroyer") return 22.f;
    if (id == "blighter")  return 18.f;
    if (id == "overseer")  return 14.f;
    if (id == "boss")      return 30.f;
    return 14.f;
}

// ── Constructeur avec scaling par vague ───────────────────────────────────────
Enemy::Enemy(const EnemyData& data, int waveNumber)
    : _id(data.id), _typeDegats(data.typeDegats),
      _reward(data.drop), _pathIndex(0), _alive(true)
{
    float scale = 1.f + data.scalingPv * (waveNumber - 1);
    _hp    = (int)(data.pv * scale);
    _maxHp = _hp;
    _speed = data.vitesse * 40.f;

    float r = radiusForId(data.id);
    _shape.setRadius(r);
    _shape.setOrigin({r, r});
    _shape.setFillColor(colorForId(data.id));
    _shape.setOutlineColor(sf::Color(0, 0, 0, 180));
    _shape.setOutlineThickness(1.5f);

    float barW = r * 2.4f;
    _hpBarBg.setSize({barW, 5.f});
    _hpBarBg.setFillColor(sf::Color(160, 0, 0));
    _hpBar.setFillColor(sf::Color(0, 210, 0));
}

void Enemy::move(const std::vector<sf::Vector2f>& path, float dt) {
    if (_pathIndex >= (int)path.size()) return;
    sf::Vector2f dir = path[_pathIndex] - _shape.getPosition();
    float dist = std::hypot(dir.x, dir.y);
    if (dist < 2.f) ++_pathIndex;
    else             _shape.move((dir / dist) * _speed * dt);
}

void Enemy::takeDamage(int dmg) {
    _hp -= dmg;
    if (_hp <= 0) { _hp = 0; _alive = false; }
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(_shape);
    float barW = _shape.getRadius() * 2.4f;
    sf::Vector2f pos = _shape.getPosition();
    _hpBarBg.setPosition({pos.x - barW / 2.f, pos.y - _shape.getRadius() - 10.f});
    window.draw(_hpBarBg);
    _hpBar.setSize({barW * ((float)_hp / _maxHp), 5.f});
    _hpBar.setPosition(_hpBarBg.getPosition());
    window.draw(_hpBar);
}

void         Enemy::kill()                       { _alive = false; _reward = 0; }
void         Enemy::clearReward()                { _reward = 0; }
bool         Enemy::isAlive()        const       { return _alive; }
int          Enemy::getReward()      const       { return _reward; }
int          Enemy::getPathIndex()   const       { return _pathIndex; }
sf::Vector2f Enemy::getPosition()    const       { return _shape.getPosition(); }
std::string  Enemy::getId()          const       { return _id; }
std::string  Enemy::getTypeDegats()  const       { return _typeDegats; }
void         Enemy::setPosition(sf::Vector2f p)  { _shape.setPosition(p); }

// ── EnemyFactory ──────────────────────────────────────────────────────────────
void EnemyFactory::loadFromJson(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        // Essai chemin alternatif
        std::ifstream f2("../Assets/Data/enemies.json");
        if (!f2.is_open()) return;
        auto j = nlohmann::json::parse(f2);
        for (auto& e : j["enemies"]) {
            EnemyData d;
            d.id         = e["id"].get<std::string>();
            d.name       = e["name"].get<std::string>();
            d.pv         = e.value("pv", 100);
            d.vitesse    = (float)e.value("vitesse", 2);
            d.dps        = e.value("dps", 10);
            d.drop       = e.value("drop", 10);
            d.scalingPv  = (float)e.value("scaling_pv", 0.15);
            d.typeDegats = e.value("type_degats", "single");
            _catalog[d.id] = d;
        }
        return;
    }
    auto j = nlohmann::json::parse(f);
    _catalog.clear();
    for (auto& e : j["enemies"]) {
        EnemyData d;
        d.id         = e["id"].get<std::string>();
        d.name       = e["name"].get<std::string>();
        d.pv         = e.value("pv", 100);
        d.vitesse    = (float)e.value("vitesse", 2);
        d.dps        = e.value("dps", 10);
        d.drop       = e.value("drop", 10);
        d.scalingPv  = (float)e.value("scaling_pv", 0.15);
        d.typeDegats = e.value("type_degats", "single");
        _catalog[d.id] = d;
    }
}

std::unique_ptr<Enemy> EnemyFactory::create(const std::string& id, int wave) {
    // Fallback si le catalogue est vide
    if (_catalog.empty()) {
        EnemyData d; d.id = id; d.name = id;
        d.pv = 100; d.vitesse = 2; d.dps = 10; d.drop = 10;
        return std::make_unique<Enemy>(d, wave);
    }
    auto it = _catalog.find(id);
    if (it == _catalog.end()) it = _catalog.begin();
    const EnemyData& d = it->second;
    if (id == "scout")     return std::make_unique<Scout>(d, wave);
    if (id == "infected")  return std::make_unique<Infected>(d, wave);
    if (id == "destroyer") return std::make_unique<Destroyer>(d, wave);
    if (id == "blighter")  return std::make_unique<Blighter>(d, wave);
    if (id == "overseer")  return std::make_unique<Overseer>(d, wave);
    if (id == "boss")      return std::make_unique<BossEnemy>(d, wave);
    return std::make_unique<Enemy>(d, wave);
}

bool EnemyFactory::hasId(const std::string& id) { return _catalog.count(id) > 0; }
const std::map<std::string, EnemyData>& EnemyFactory::getCatalog() { return _catalog; }
