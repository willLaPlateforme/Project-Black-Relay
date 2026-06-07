#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>

class Scores {
public:
    Scores(sf::RenderWindow& window, sf::Font& font);

    void addScore(const std::string& name, int score);
    void handleEvents(bool& back);
    void render();
    void rebind(sf::RenderWindow& window);

    // Sauvegarde et chargement depuis un fichier
    void saveToFile(const std::string& path = "Assets/Data/scores.json");
    void loadFromFile(const std::string& path = "Assets/Data/scores.json");

private:
    sf::RenderWindow* _window;
    sf::Font&         _font;

    struct Entry {
        std::string name;
        int         score;
        // Tri décroissant par score
        bool operator>(const Entry& o) const { return score > o.score; }
    };
    std::vector<Entry> _entries;

    static const int MAX_ENTRIES = 10; // garder les 10 meilleurs scores
};