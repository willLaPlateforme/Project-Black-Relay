#pragma once
#include <SFML/Graphics.hpp>
#include "../../../Menu/Includes/Options.hpp"

// Classe Affichage_Options : gere uniquement l'affichage de l'ecran des options
// Recoit une reference a Options (logique) pour lire les donnees a afficher

class Affichage_Options {

    public:
        Affichage_Options(sf::RenderWindow& window, sf::Font& font);

        // Dessine l'ecran des options complet
        void render(const Options& options);

    private:
        sf::RenderWindow&   _window;
        sf::Font&           _font;

        sf::Vector2f  _mousePos;

        // Dessine un bouton de difficulte avec son etat (selectionne, survole)
        void _drawDiffBtn(sf::FloatRect rect, const std::string& label,
                          Difficulte diff, Difficulte current);
};