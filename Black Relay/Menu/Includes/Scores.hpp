#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Scores {
public:
    Scores(sf::RenderWindow& window, sf::Font& font);
    void addScore(const std::string& name, int score);
    void handleEvents(bool& back);
    void render();

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    struct Entry { std::string name; int score; };
    std::vector<Entry> _entries;
};