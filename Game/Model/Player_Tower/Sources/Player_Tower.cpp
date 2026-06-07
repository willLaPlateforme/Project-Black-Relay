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
    if (id == "elementalist") return sf::Color(155, 55,  210);
    if (id == "infiltrator")  return sf::Color(60,  180, 180);
    if (id == "berserker")    return sf::Color(220, 60,  60);
    if (id == "black_relay")  return sf::Color(220, 30,  30);
    return sf::Color(140, 140, 140);
}

float Player_Tower::scaleForId(const std::string& id) {
    (void)id;
    // Les sprites sont déjà redimensionnés à 48x48/64x64 à la génération
    return 1.0f;
}

// ── Constructeur ──────────────────────────────────────────────────────────────
Player_Tower::Player_Tower(const TowerData& data, float x, float y)
    : _data(data), _level(1), _hp(data.pv), _maxHp(data.pv), _cooldown(0.f)
{
    _range    = porteeToPixels(data.portee);
    _damage   = data.dps;
    _fireRate = std::max(0.3f, std::min(2.f, _damage / 30.f));
    _spriteScale = scaleForId(data.id);

    // Fallback rectangle
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
    _rangeCircle.setFillColor(sf::Color(220, 30, 30, 15));
    _rangeCircle.setOutlineColor(sf::Color(220, 30, 30, 160));
    _rangeCircle.setOutlineThickness(1.5f);

    // Chargement sprites
    _loadSprites();
}

// ── Chargement des sprites depuis Assets/Sprites/{id}/ ───────────────────────
void Player_Tower::_loadSprites() {
    struct AnimDef { TowerAnimState state; std::string name; int frames; float fps; };

    std::vector<AnimDef> defs;
    std::string id = _data.id;

    if (id == "sniper" || id == "vanguard") {
        // Sprites plus grands visuellement → FPS plus bas pour paraître cohérent
        defs = {
            {TowerAnimState::IDLE,  "idle",  4, 5.f},
            {TowerAnimState::WALK,  "walk",  4, 6.f},
            {TowerAnimState::DEATH, "death", 4, 5.f},
        };
    } else if (id == "guardian"  || id == "assault"    || id == "heavy_gunner" ||
               id == "medic"     || id == "elementalist"|| id == "infiltrator"  ||
               id == "berserker") {
        defs = {
            {TowerAnimState::IDLE,  "idle",  4, 8.f},
            {TowerAnimState::WALK,  "walk",  4, 10.f},
            {TowerAnimState::DEATH, "death", 4, 8.f},
        };
    } else {
        return; // black_relay ou inconnu : fallback rectangle
    }

    std::string base = "Assets/Sprites/" + id + "/";

    for (auto& def : defs) {
        TowerAnimation anim;
        anim.fps = def.fps;
        bool ok  = true;

        for (int i = 0; i < def.frames; ++i) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%02d", i);
            sf::Texture tex;
            if (!tex.loadFromFile(base + def.name + "_" + buf + ".png")) {
                ok = false; break;
            }
            anim.frames.push_back(std::move(tex));
        }

        if (!ok) { _animations.clear(); return; }
        _animations[def.state] = std::move(anim);
    }

    // Init sprite avec première frame IDLE
    if (_animations.count(TowerAnimState::IDLE) &&
        !_animations[TowerAnimState::IDLE].frames.empty()) {
        auto& tex = _animations[TowerAnimState::IDLE].frames[0];
        _sprite.emplace(tex);
        auto sz = tex.getSize();
        _sprite->setOrigin({sz.x / 2.f, sz.y / 2.f});
        _sprite->setScale({_spriteScale, _spriteScale});
        _sprite->setPosition(_shape.getPosition());
        _spriteLoaded = true;
    }
}

// ── Changement d'animation ────────────────────────────────────────────────────
void Player_Tower::_setAnim(TowerAnimState s) {
    if (!_spriteLoaded) return;
    if (_currentAnim == s) return;
    if (!_animations.count(s) || _animations[s].frames.empty()) return;

    _currentAnim  = s;
    _currentFrame = 0;
    _frameTimer   = 0.f;

    auto& tex = _animations[s].frames[0];
    _sprite->setTexture(tex);
    auto sz = tex.getSize();
    _sprite->setOrigin({sz.x / 2.f, sz.y / 2.f});
}

// ── Mise à jour de l'animation ────────────────────────────────────────────────
void Player_Tower::_updateAnimation(float dt) {
    if (!_spriteLoaded || !_sprite.has_value()) return;
    if (!_animations.count(_currentAnim)) return;

    auto& anim = _animations[_currentAnim];
    if (anim.frames.empty()) return;

    _frameTimer += dt;
    float frameDur = 1.f / anim.fps;

    if (_frameTimer >= frameDur) {
        _frameTimer -= frameDur;
        _currentFrame = (_currentFrame + 1) % (int)anim.frames.size();
        _sprite->setTexture(anim.frames[_currentFrame]);
    }
}

// ── Update : attaque + animation ─────────────────────────────────────────────
void Player_Tower::update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies) {
    _cooldown -= dt;

    Enemy* t = _findTarget(enemies);
    if (t) {
        _setAnim(TowerAnimState::WALK); // animation "active" quand ennemi en portée
        if (_cooldown <= 0.f) {
            attack(t, enemies);
            _cooldown = 1.f / _fireRate;
        }
    } else {
        _setAnim(TowerAnimState::IDLE);
    }

    _updateAnimation(dt);
}

// ── Attaque ───────────────────────────────────────────────────────────────────
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

// ── Upgrade ───────────────────────────────────────────────────────────────────
void Player_Tower::upgrade() {
    if (_level >= 3) return;
    ++_level;
    _damage    = (int)(_damage * 1.5f);
    _range    *= 1.15f;
    _fireRate *= 1.2f;
    _rangeCircle.setRadius(_range);
    _rangeCircle.setOrigin({_range, _range});
    if (_spriteLoaded && _sprite)
        _sprite->setScale({_spriteScale * (1.f + 0.1f * _level),
                           _spriteScale * (1.f + 0.1f * _level)});
}

// ── Dessin ────────────────────────────────────────────────────────────────────
void Player_Tower::draw(sf::RenderWindow& window, bool showRange) {
    if (showRange) window.draw(_rangeCircle);

    if (_spriteLoaded && _sprite.has_value()) {
        _sprite->setPosition(_shape.getPosition());
        window.draw(*_sprite);
    } else {
        window.draw(_shape);
    }
}

// ── Getters ───────────────────────────────────────────────────────────────────
sf::Vector2f       Player_Tower::getPosition()    const { return _shape.getPosition(); }
int                Player_Tower::getLevel()        const { return _level; }
int                Player_Tower::getCost()         const { return _data.shards * _level; }
const std::string& Player_Tower::getId()           const { return _data.id; }
const std::string& Player_Tower::getName()         const { return _data.name; }
const std::string& Player_Tower::getUpgradeDesc()  const {
    return (_level == 1) ? _data.upgrade1 : _data.upgrade2;
}
void  Player_Tower::takeDamage(int dmg) { _hp -= dmg; if (_hp < 0) _hp = 0; }
int   Player_Tower::getHp()    const { return _hp; }
int   Player_Tower::getMaxHp() const { return _maxHp; }
float Player_Tower::getRange()  const { return _range; }

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
            d.id=u["id"]; d.name=u["name"];
            d.pv=u.value("pv",100); d.shards=u.value("shards",50);
            d.dps=u.value("dps",20); d.portee=u.value("portee","moyenne");
            d.typeDegats=u.value("type_degats","single");
            d.upgrade1=u.value("upgrade1",""); d.upgrade2=u.value("upgrade2","");
            d.unique=u.value("unique",false);
            _catalog[d.id]=d;
        }
        return;
    }
    auto j = nlohmann::json::parse(f);
    _catalog.clear();
    for (auto& u : j["units"]) {
        TowerData d;
        d.id=u["id"]; d.name=u["name"];
        d.pv=u.value("pv",100); d.shards=u.value("shards",50);
        d.dps=u.value("dps",20); d.portee=u.value("portee","moyenne");
        d.typeDegats=u.value("type_degats","single");
        d.upgrade1=u.value("upgrade1",""); d.upgrade2=u.value("upgrade2","");
        d.unique=u.value("unique",false);
        _catalog[d.id]=d;
    }
}

std::unique_ptr<Player_Tower> TowerFactory::create(const std::string& id, float x, float y) {
    if (_catalog.empty()) {
        TowerData d; d.id=id; d.name=id; d.shards=50; d.dps=20;
        return std::make_unique<Player_Tower>(d, x, y);
    }
    auto it = _catalog.find(id);
    if (it == _catalog.end()) it = _catalog.begin();
    return std::make_unique<Player_Tower>(it->second, x, y);
}

std::unique_ptr<Player_Tower> TowerFactory::createCompat(const std::string& type, float x, float y) {
    std::string id = type;
    if (type == "archer") id = "sniper";
    if (type == "cannon") id = "heavy_gunner";
    if (type == "magic")  id = "elementalist";
    return create(id, x, y);
}

bool TowerFactory::hasId(const std::string& id) { return _catalog.count(id) > 0; }
const std::map<std::string, TowerData>& TowerFactory::getCatalog() { return _catalog; }