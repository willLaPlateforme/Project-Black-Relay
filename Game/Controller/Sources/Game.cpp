#include "../Includes/Game.hpp"

Controller::Controller(sf::RenderWindow& window, Game& game)
    : _window(window), _game(game) {}

void Controller::handleEvents() {
    while (auto event = _window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) _window.close();

        if (auto* key = event->getIf<sf::Event::KeyPressed>())
            _onKeyPressed(key->code);

        if (auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (click->button == sf::Mouse::Button::Left)
                _onMouseClick((float)click->position.x, (float)click->position.y);
            if (click->button == sf::Mouse::Button::Right)
                _onMouseRightClick((float)click->position.x, (float)click->position.y);
        }
    }
}

void Controller::_onKeyPressed(sf::Keyboard::Key key) {
    using K = sf::Keyboard::Key;
    switch (key) {
        case K::Escape:
            if (_game.getState() == GameState::PLAYING ||
                _game.getState() == GameState::PAUSED)
                _game.setState(GameState::MENU);   // signal retour menu
            else
                _window.close();
            break;
        case K::Enter:
            if (_game.getState() == GameState::VICTORY ||
                _game.getState() == GameState::DEFEAT)
                _game.setState(GameState::MENU);   // retour menu après fin
            break;
        case K::P:
            if (_game.getState() == GameState::PLAYING)
                _game.setState(GameState::PAUSED);
            else if (_game.getState() == GameState::PAUSED)
                _game.setState(GameState::PLAYING);
            break;
        case K::Space:
            _game.launchNextWave();
            break;
        case K::M:
            _game.setSoundEnabled(!_game.isSoundEnabled());
            break;
        case K::Num1: _game.setSelectedTower("sniper");       break;
        case K::Num2: _game.setSelectedTower("heavy_gunner"); break;
        case K::Num3: _game.setSelectedTower("elementalist"); break;
        case K::Num4: _game.setSelectedTower("vanguard");     break;
        case K::Num5: _game.setSelectedTower("guardian");     break;
        case K::Num6: _game.setSelectedTower("assault");      break;
        default: break;
    }
}

void Controller::_onMouseClick(float x, float y) {
    if (_game.getState() != GameState::PLAYING) return;
    if (y < 55.f || y > 950.f) return;
    _game.placeTower(x, y);
}

void Controller::_onMouseRightClick(float x, float y) {
    if (_game.getState() != GameState::PLAYING) return;
    if (y < 55.f || y > 950.f) return;
    _game.upgradeTower(x, y);
}
