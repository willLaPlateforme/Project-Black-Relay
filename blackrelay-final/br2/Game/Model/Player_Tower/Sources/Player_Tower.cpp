#include "../Includes/Player_Tower.hpp"
#include "../../Enemy/Includes/Ennemy.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>

float Player_Tower::porteeToPixels(const std::string& p) {
    if (p == "courte")      return 90.f;
    if (p == "moyenne")     return 140.f;
    if (p == "longue")      return 190.f;
    if (p == "tres_longue") return 260.f;
    return 140.f;
}

sf::Color Player_Tower::colorForId(const std::string& id) {
    if (id == "vanguard")     return sf::Color(200, 200, 80);
    if (id == "guardian")     return sf::Color(80,  160, 220);
    if (id == "sniper")       return sf::Color(200, 165, 55);
    if (id == "assault")      return sf::Color(200, 80,  80);
    if (id == "heavy_gunner") return sf::Color(85,  85,  90);
    if (id == "medic")        return sf::Color(80,  220, 120);
    if (id == "tactician")    return sf::Color(180, 120, 220);
    if (id == "elementalist") return sf::Color(155, 55,  210);
    if (id == "infiltrator")  return sf::Color(60,  180, 180);
    if (id == "engineer")     return sf::Color(220, 140, 60);
    if (id == "exorcist")     return sf::Color(220, 220, 255);
    if (id == "berserker")    return sf::Color(220, 60,  60);
    if (id == "sentinel")     return sf::Color(100, 140, 220);
    if (id == "black_relay")  return sf::Color(220, 30,  30);
    return sf::Color(140, 140, 140);
}

Player_Tower::Player_Tower(const TowerData& data, float x, float y)
    : _data(data), _level(1), _cooldown(0.f)
{
    _range    = porteeToPixels(data.portee);
    _damage   = data.dps;
    _fireRate = std::max(0.3f, std::min(2.f, _damage / 30.f));

    float size = (data.id == "black_relay") ? 50.f : 36.f;
    _shape.setSize({size, size});
    _shape.setOrigin({size / 2.f, size / 2.f});
    _shape.setPosition({x, y});
    _shape.setFillColor(colorForId(data.id));
    _shape.setOutlineColor(sf::Color(255, 255, 255, 80));
    _shape.setOutlineThickness(1.5f);

    _rangeCircle.setRadius(_range);
    _rangeCircle.setOrigin({_range, _range});
    _rangeCircle.setPosition({x, y});
    _rangeCircle.setFillColor(sf::Color::Transparent);
    _rangeCircle.setOutlineColor(sf::Color(255, 255, 255, 40));
    _rangeCircle.setOutlineThickness(1.f);
}

void Player_Tower::update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies) {
    _cooldown -= dt;
    if (_cooldown > 0.f) return;
    Enemy* t = _findTarget(enemies);
    if (t) { attack(t, enemies); _cooldown = 1.f / _fireRate; }
}

void Player_Tower::attack(Enemy* target, std::vector<std::unique_ptr<Enemy>>& all) {
    if (_data.typeDegats == "aoe") {
        sf::Vector2f tpos = target->getPosition();
        for (auto& e : all) {
            if (!e->isAlive()) continue;
            float dx = e->getPosition().x - tpos.x;
            float dy = e->getPosition().y - tpos.y;
            if (std::hypot(dx, dy) <= 60.f)
                e->takeDamage(_damage);
        }
    } else {
        target->takeDamage(_damage);
    }
}

void Player_Tower::upgrade() {
    if (_level >= 3) return;
    ++_level;
    _damage    = (int)(_damage * 1.5f);
    _range    *= 1.15f;
    _fireRate *= 1.2f;
    _rangeCircle.setRadius(_range);
    _rangeCircle.setOrigin({_range, _range});
}

void Player_Tower::draw(sf::RenderWindow& window, bool showRange) {
    if (showRange) window.draw(_rangeCircle);
    window.draw(_shape);
}

sf::Vector2f       Player_Tower::getPosition()    const { return _shape.getPosition(); }
int                Player_Tower::getLevel()        const { return _level; }
int                Player_Tower::getCost()         const { return _data.shards * _level; }
const std::string& Player_Tower::getId()           const { return _data.id; }
const std::string& Player_Tower::getName()         const { return _data.name; }
const std::string& Player_Tower::getUpgradeDesc()  const {
    return (_level == 1) ? _data.upgrade1 : _data.upgrade2;
}

Enemy* Player_Tower::_findTarget(std::vector<std::unique_ptr<Enemy>>& enemies) {
    Enemy* best = nullptr; int bestIdx = -1;
    sf::Vector2f pos = _shape.getPosition();
    for (auto& e : enemies) {
        if (!e->isAlive()) continue;
        float dx = e->getPosition().x - pos.x;
        float dy = e->getPosition().y - pos.y;
        if (std::hypot(dx, dy) <= _range && e->getPathIndex() > bestIdx) {
            bestIdx = e->getPathIndex(); best = e.get();
        }
    }
    return best;
}

// ── TowerFactory ──────────────────────────────────────────────────────────────
void TowerFactory::loadFromJson(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::ifstream f2("../Assets/Data/units.json");
        if (!f2.is_open()) return;
        auto j = nlohmann::json::parse(f2);
        _catalog.clear();
        for (auto& u : j["units"]) {
            TowerData d;
            d.id         = u["id"].get<std::string>();
            d.name       = u["name"].get<std::string>();
            d.pv         = u.value("pv", 100);
            d.shards     = u.value("shards", 50);
            d.dps        = u.value("dps", 20);
            d.portee     = u.value("portee", "moyenne");
            d.typeDegats = u.value("type_degats", "single");
            d.upgrade1   = u.value("upgrade1", "");
            d.upgrade2   = u.value("upgrade2", "");
            d.unique     = u.value("unique", false);
            _catalog[d.id] = d;
        }
        return;
    }
    auto j = nlohmann::json::parse(f);
    _catalog.clear();
    for (auto& u : j["units"]) {
        TowerData d;
        d.id         = u["id"].get<std::string>();
        d.name       = u["name"].get<std::string>();
        d.pv         = u.value("pv", 100);
        d.shards     = u.value("shards", 50);
        d.dps        = u.value("dps", 20);
        d.portee     = u.value("portee", "moyenne");
        d.typeDegats = u.value("type_degats", "single");
        d.upgrade1   = u.value("upgrade1", "");
        d.upgrade2   = u.value("upgrade2", "");
        d.unique     = u.value("unique", false);
        _catalog[d.id] = d;
    }
}

std::unique_ptr<Player_Tower> TowerFactory::create(const std::string& id, float x, float y) {
    if (_catalog.empty()) {
        TowerData d; d.id = id; d.name = id; d.shards = 50; d.dps = 20;
        return std::make_unique<Player_Tower>(d, x, y);
    }
    auto it = _catalog.find(id);
    if (it == _catalog.end()) it = _catalog.begin();
    return std::make_unique<Player_Tower>(it->second, x, y);
}

std::unique_ptr<Player_Tower> TowerFactory::createCompat(const std::string& type,
                                                           float x, float y) {
    // Touche 1 → sniper, 2 → heavy_gunner, 3 → elementalist
    std::string id = type;
    if (type == "archer") id = "sniper";
    if (type == "cannon") id = "heavy_gunner";
    if (type == "magic")  id = "elementalist";
    return create(id, x, y);
}

bool TowerFactory::hasId(const std::string& id) { return _catalog.count(id) > 0; }
const std::map<std::string, TowerData>& TowerFactory::getCatalog() { return _catalog; }
