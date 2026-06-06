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

float Enemy::scaleForId(const std::string& id) {
    if (id == "scout")     return 0.35f;
    if (id == "infected")  return 0.40f;
    if (id == "destroyer") return 0.55f;
    if (id == "blighter")  return 0.45f;
    if (id == "overseer")  return 0.38f;
    return 0.40f;
}

// ── Constructeur ──────────────────────────────────────────────────────────────
Enemy::Enemy(const EnemyData& data, int waveNumber)
    : _id(data.id), _typeDegats(data.typeDegats),
      _reward(data.drop), _pathIndex(0), _alive(true),
      _dps(data.dps), _deathAnimDone(false),
      _currentAnim(AnimState::WALK), _currentFrame(0),
      _frameTimer(0.f), _spriteLoaded(false),
      _scale(scaleForId(data.id))
{
    float scale = 1.f + data.scalingPv * (waveNumber - 1);
    _hp    = (int)(data.pv * scale);
    _maxHp = _hp;
    _speed = data.vitesse * 40.f;

    // Chargement des sprites (silencieux si pas trouvés)
    _loadSprites();

    // Fallback cercle coloré
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

// ── Chargement des sprites ────────────────────────────────────────────────────
void Enemy::_loadSprites() {
    struct AnimDef { AnimState state; std::string folder; int frames; float fps; };
    std::vector<AnimDef> defs;

    if      (_id == "scout")     defs = {{AnimState::WALK,"walk",6,10.f},{AnimState::DEATH,"death",4,8.f}};
    else if (_id == "infected")  defs = {{AnimState::WALK,"walk",8,10.f},{AnimState::DEATH,"death",4,8.f}};
    else if (_id == "destroyer") defs = {{AnimState::WALK,"walk",5, 8.f},{AnimState::DEATH,"death",5,8.f}};
    else if (_id == "blighter")  defs = {{AnimState::WALK,"walk",8,10.f},{AnimState::DEATH,"death",4,8.f}};
    else if (_id == "overseer")  defs = {{AnimState::WALK,"float",4,8.f},{AnimState::DEATH,"death",3,8.f}};
    else return; // boss : pas de sprite

    std::string base = "Assets/Sprites/" + _id + "/";

    for (auto& def : defs) {
        Animation anim;
        anim.fps = def.fps;
        bool ok  = true;

        for (int i = 0; i < def.frames; ++i) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%02d", i);
            sf::Texture tex;
            if (!tex.loadFromFile(base + def.folder + "_" + buf + ".png")) {
                ok = false; break;
            }
            anim.frames.push_back(std::move(tex));
        }

        if (!ok) { _animations.clear(); return; }
        _animations[def.state] = std::move(anim);
    }

    // Initialiser le sprite avec la première frame de WALK
    if (_animations.count(AnimState::WALK) &&
        !_animations[AnimState::WALK].frames.empty()) {
        auto& tex = _animations[AnimState::WALK].frames[0];
        _sprite.emplace(tex);
        auto sz = tex.getSize();
        _sprite->setOrigin({sz.x / 2.f, sz.y / 2.f});
        _sprite->setScale({_scale, _scale});
        _spriteLoaded = true;
    }
}

// ── Changement d'animation ────────────────────────────────────────────────────
void Enemy::_setAnim(AnimState s) {
    if (!_spriteLoaded) return;
    if (_currentAnim == s) return;
    if (!_animations.count(s)) return;
    if (_animations[s].frames.empty()) return;

    _currentAnim  = s;
    _currentFrame = 0;
    _frameTimer   = 0.f;

    auto& tex = _animations[s].frames[0];
    _sprite->setTexture(tex);
    auto sz = tex.getSize();
    _sprite->setOrigin({sz.x / 2.f, sz.y / 2.f});
}

// ── Mise à jour de l'animation ────────────────────────────────────────────────
void Enemy::updateAnimation(float dt) {
    if (!_spriteLoaded || !_sprite.has_value()) return;
    if (!_animations.count(_currentAnim)) return;

    auto& anim = _animations[_currentAnim];
    if (anim.frames.empty()) return;

    _frameTimer += dt;
    float frameDur = 1.f / anim.fps;

    if (_frameTimer >= frameDur) {
        _frameTimer -= frameDur;

        if (_currentAnim == AnimState::DEATH) {
            if (_currentFrame < (int)anim.frames.size() - 1)
                ++_currentFrame;
            else
                _deathAnimDone = true;
        } else {
            _currentFrame = (_currentFrame + 1) % (int)anim.frames.size();
        }

        if (_currentFrame < (int)anim.frames.size()) {
            _sprite->setTexture(anim.frames[_currentFrame]);
        }
    }
}

// ── Déplacement ───────────────────────────────────────────────────────────────
void Enemy::move(const std::vector<sf::Vector2f>& path, float dt) {
    if (_pathIndex >= (int)path.size()) return;

    sf::Vector2f dir  = path[_pathIndex] - _shape.getPosition();
    float        dist = std::hypot(dir.x, dir.y);

    if (dist < 2.f) {
        ++_pathIndex;
    } else {
        _shape.move((dir / dist) * _speed * dt);
        if (_spriteLoaded && _sprite.has_value()) {
            _sprite->setScale({
                dir.x < 0 ? -_scale : _scale,
                _scale
            });
        }
    }

    updateAnimation(dt);
}

// ── Dégâts ────────────────────────────────────────────────────────────────────
void Enemy::takeDamage(int dmg) {
    _hp -= dmg;
    if (_hp <= 0) {
        _hp    = 0;
        _alive = false;
        if (_spriteLoaded && _animations.count(AnimState::DEATH))
            _setAnim(AnimState::DEATH);
    }
}

// ── Dessin ────────────────────────────────────────────────────────────────────
void Enemy::draw(sf::RenderWindow& window) {
    sf::Vector2f pos = _shape.getPosition();

    if (_spriteLoaded && _sprite.has_value()) {
        _sprite->setPosition(pos);
        window.draw(*_sprite);
    } else {
        window.draw(_shape);
    }

    // Barre de vie
    float r    = _shape.getRadius();
    float barW = r * 2.4f;
    _hpBarBg.setPosition({pos.x - barW/2.f, pos.y - r - 10.f});
    window.draw(_hpBarBg);
    _hpBar.setSize({barW * ((float)_hp / _maxHp), 5.f});
    _hpBar.setPosition(_hpBarBg.getPosition());
    window.draw(_hpBar);
}

// ── Getters ───────────────────────────────────────────────────────────────────
void         Enemy::kill()                       { _alive = false; _reward = 0; if (_spriteLoaded && _animations.count(AnimState::DEATH)) _setAnim(AnimState::DEATH); }
void         Enemy::clearReward()                { _reward = 0; }
bool         Enemy::isAlive()        const       { return _alive; }
bool         Enemy::isDead()         const       { return !_alive && _deathAnimDone; }
int          Enemy::getReward()      const       { return _reward; }
int          Enemy::getPathIndex()   const       { return _pathIndex; }
int          Enemy::getDps()         const       { return _dps; }
sf::Vector2f Enemy::getPosition()    const       { return _shape.getPosition(); }
std::string  Enemy::getId()          const       { return _id; }
std::string  Enemy::getTypeDegats()  const       { return _typeDegats; }
void         Enemy::setPosition(sf::Vector2f p)  { _shape.setPosition(p); }

// ── EnemyFactory ──────────────────────────────────────────────────────────────
void EnemyFactory::loadFromJson(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::ifstream f2("../Assets/Data/enemies.json");
        if (!f2.is_open()) return;
        auto j = nlohmann::json::parse(f2);
        _catalog.clear();
        for (auto& e : j["enemies"]) {
            EnemyData d;
            d.id=e["id"]; d.name=e["name"];
            d.pv=(int)e.value("pv",100); d.vitesse=(float)e.value("vitesse",2);
            d.dps=(int)e.value("dps",10); d.drop=(int)e.value("drop",10);
            d.scalingPv=(float)e.value("scaling_pv",0.15);
            d.typeDegats=e.value("type_degats","single");
            _catalog[d.id]=d;
        }
        return;
    }
    auto j = nlohmann::json::parse(f);
    _catalog.clear();
    for (auto& e : j["enemies"]) {
        EnemyData d;
        d.id=e["id"]; d.name=e["name"];
        d.pv=(int)e.value("pv",100); d.vitesse=(float)e.value("vitesse",2);
        d.dps=(int)e.value("dps",10); d.drop=(int)e.value("drop",10);
        d.scalingPv=(float)e.value("scaling_pv",0.15);
        d.typeDegats=e.value("type_degats","single");
        _catalog[d.id]=d;
    }
}

std::unique_ptr<Enemy> EnemyFactory::create(const std::string& id, int wave) {
    if (_catalog.empty()) {
        EnemyData d; d.id=id; d.name=id; d.pv=100; d.vitesse=2; d.dps=10; d.drop=10;
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