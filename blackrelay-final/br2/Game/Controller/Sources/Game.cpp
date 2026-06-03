#include "../Includes/Game.hpp"

Controller::Controller(sf::RenderWindow& window, Game& game)
    : _window(window), _game(game) {}

void Controller::handleEvents() {
    while (auto event = _window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) _window.close();
        if (auto* key = event->getIf<sf::Event::KeyPressed>())
            _onKeyPressed(key->code);
        if (auto* click = event->getIf<sf::Event::MouseButtonPressed>())
            if (click->button == sf::Mouse::Button::Left)
                _onMouseClick((float)click->position.x, (float)click->position.y);
    }
}

void Controller::_onKeyPressed(sf::Keyboard::Key key) {
    using K = sf::Keyboard::Key;
    switch (key) {
        case K::Escape:
            if (_game.getState() == GameState::PLAYING ||
                _game.getState() == GameState::PAUSED)
                _game.setState(GameState::MENU);
            else _window.close();
            break;
        case K::Enter:
            if (_game.getState() == GameState::MENU    ||
                _game.getState() == GameState::VICTORY ||
                _game.getState() == GameState::DEFEAT)
                _game.startGame();
            break;
        case K::P:
            if (_game.getState() == GameState::PLAYING)
                _game.setState(GameState::PAUSED);
            else if (_game.getState() == GameState::PAUSED)
                _game.setState(GameState::PLAYING);
            break;
        case K::Space: _game.launchNextWave();           break;
        case K::Num1:  _game.setSelectedTower("archer"); break;
        case K::Num2:  _game.setSelectedTower("cannon"); break;
        case K::Num3:  _game.setSelectedTower("magic");  break;
        default: break;
    }
}

void Controller::_onMouseClick(float x, float y) {
    if (_game.getState() == GameState::PLAYING && y > 50.f)
        _game.placeTower(x, y);
}
