#pragma once
#include <SFML/Graphics.hpp>
#include "../../../Menu/Includes/Options.hpp"

// Classe Affichage_Options : gère uniquement l'affichage de l'écran des options
class Affichage_Options {
public:
    Affichage_Options(sf::RenderWindow& window, sf::Font& font);
    void render(const Options& options);

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    sf::Vector2f      _mousePos;

    void _drawDiffBtn(sf::FloatRect rect, const std::string& label,
                      Difficulte diff, Difficulte current);
};
