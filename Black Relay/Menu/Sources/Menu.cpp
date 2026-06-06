#include "../Includes/Menu.hpp"

Menu::Menu()
    : _selectedIndex(0), _activatedIndex(-1), _hoveredIndex(-1)
{
    if (_music.openFromFile("Assets/Ost/Menu/Black Relay(Menu Ost).mp3")) {
        _music.setLooping(true);
        _music.play();
    }

    _items = {
        {"CONTINUER"},
        {"NOUVELLE PARTIE"},
        {"OPERATEURS"},
        {"ARSENAL"},
        {"RECHERCHE"},
        {"DOSSIER"},
        {"PARAMETRES"},
        {"CREDITS"},
        {"QUITTER"}
    };
}

int Menu::_getItemAtPos(sf::Vector2f pos) const {
    float startY = 295.f, itemH = 35.f;
    for (int i = 0; i < (int)_items.size(); ++i) {
        float y = startY + i * itemH;
        sf::FloatRect rect({25.f, y - 4.f}, {285.f, 30.f});
        if (rect.contains(pos))
            return i;
    }
    return -1;
}

void Menu::update(float animTimer, sf::Vector2f mousePos) {
    _hoveredIndex = _getItemAtPos(mousePos);
    if (_hoveredIndex >= 0)
        _selectedIndex = _hoveredIndex;
}

void Menu::handleKeyPressed(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Up:
            _selectedIndex = (_selectedIndex - 1 + (int)_items.size()) % (int)_items.size();
            break;
        case sf::Keyboard::Key::Down:
            _selectedIndex = (_selectedIndex + 1) % (int)_items.size();
            break;
        case sf::Keyboard::Key::Enter:
            _activatedIndex = _selectedIndex;
            break;
        case sf::Keyboard::Key::Escape:
            _activatedIndex = QUITTER;
            break;
        default: break;
    }
}

void Menu::handleMouseMoved(sf::Vector2f pos) {
    _hoveredIndex = _getItemAtPos(pos);
    if (_hoveredIndex >= 0)
        _selectedIndex = _hoveredIndex;
}

void Menu::handleMouseClicked(sf::Vector2f pos) {
    int idx = _getItemAtPos(pos);
    if (idx >= 0)
        _activatedIndex = idx;
}

void Menu::handleClosed() {
    _activatedIndex = QUITTER;
}

int  Menu::getActivated()     const { return _activatedIndex; }
void Menu::resetActivated()         { _activatedIndex = -1; }
int  Menu::getSelectedIndex() const { return _selectedIndex; }
int  Menu::getHoveredIndex()  const { return _hoveredIndex; }
const std::vector<MenuItem>& Menu::getItems() const { return _items; }
bool Menu::isMusicPlaying()   const { return _music.getStatus() == sf::Music::Status::Playing; }

void Menu::toggleSound() {
    if (_music.getStatus() == sf::Music::Status::Playing)
        _music.pause();
    else
        _music.play();
}

bool Menu::isSoundOn() const {
    return _music.getStatus() == sf::Music::Status::Playing;
}