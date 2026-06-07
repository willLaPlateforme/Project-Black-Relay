#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>

struct MenuItem { std::string label; };

// Classe Menu : gère uniquement la logique du menu principal
class Menu {
public:
    explicit Menu();

    void update(float animTimer, sf::Vector2f mousePos);
    void handleKeyPressed(sf::Keyboard::Key key);
    void handleMouseMoved(sf::Vector2f pos);
    void handleMouseClicked(sf::Vector2f pos);
    void handleClosed();

    int  getActivated()      const;
    void resetActivated();
    int  getSelectedIndex()  const;
    int  getHoveredIndex()   const;
    const std::vector<MenuItem>& getItems() const;
    bool isMusicPlaying()    const;
    void toggleSound();
    bool isSoundOn()         const;

    // Constantes des entrées du menu
    static const int CONTINUER       = 0;
    static const int NOUVELLE_PARTIE = 1;
    static const int OPERATEURS      = 2;
    static const int ARSENAL         = 3;
    static const int RECHERCHE       = 4;
    static const int DOSSIER         = 5;
    static const int PARAMETRES      = 6;
    static const int SCORES_MENU     = 7;   // <-- ajouté pour accès aux scores
    static const int CREDITS         = 8;
    static const int QUITTER         = 9;

private:
    std::vector<MenuItem> _items;
    int    _selectedIndex;
    int    _activatedIndex;
    int    _hoveredIndex;
    sf::Music _music;

    int _getItemAtPos(sf::Vector2f pos) const;
};
