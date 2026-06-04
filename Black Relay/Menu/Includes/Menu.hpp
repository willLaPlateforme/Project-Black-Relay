#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>

// Structure representant un item du menu avec son libelle
struct MenuItem { std::string label; };

// Classe Menu : gere uniquement la logique du menu principal
// (etat de navigation, selection, activation, musique)
class Menu {
public:

    explicit Menu();

    // Met a jour l'etat selon les entrees clavier/souris
    void update(float animTimer, sf::Vector2f mousePos);
    void handleKeyPressed(sf::Keyboard::Key key);
    void handleMouseMoved(sf::Vector2f pos);
    void handleMouseClicked(sf::Vector2f pos);
    void handleClosed();

    // Retourne l'index de l'action activee (-1 si aucune)
    int  getActivated() const;

    // Remet l'index active a -1 apres traitement
    void resetActivated();

    // Retourne l'index selectionne
    int  getSelectedIndex() const;

    // Retourne l'index survole
    int  getHoveredIndex() const;

    // Retourne la liste des items
    const std::vector<MenuItem>& getItems() const;

    // Retourne si la musique est active
    bool isMusicPlaying() const;

    // Constantes representant chaque entree du menu
    static const int CONTINUER       = 0;
    static const int NOUVELLE_PARTIE = 1;
    static const int OPERATEURS      = 2;
    static const int ARSENAL         = 3;
    static const int RECHERCHE       = 4;
    static const int DOSSIER         = 5;
    static const int PARAMETRES      = 6;
    static const int CREDITS         = 7;
    static const int QUITTER         = 8;

private:

    // Liste des items du menu
    std::vector<MenuItem> _items;

    // Index de l'item selectionne
    int _selectedIndex;

    // Index de l'item active (-1 si aucun)
    int _activatedIndex;

    // Index de l'item survole par la souris (-1 si aucun)
    int _hoveredIndex;

    // Musique de fond du menu
    sf::Music _music;

    // Retourne l'index de l'item situe a la position donnee (-1 si aucun)
    int _getItemAtPos(sf::Vector2f pos) const;
};