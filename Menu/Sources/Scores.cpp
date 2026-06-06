#include "../Includes/Menu.hpp"
#include <algorithm>
#include <fstream>

// ── Constructeur : charge les scores depuis le fichier ────────────────────────
Scores::Scores(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font)
{
    _loadFromFile();
}

// ── Ajout d'un score et sauvegarde ───────────────────────────────────────────
void Scores::addScore(const std::string& name, int score) {
    _entries.push_back({name, score});
    // Trier par score décroissant et garder le top 10
    std::sort(_entries.begin(), _entries.end(),
        [](const Entry& a, const Entry& b){ return a.score > b.score; });
    if ((int)_entries.size() > 10) _entries.resize(10);
    _saveToFile();
}

// ── Sauvegarde dans un fichier texte ─────────────────────────────────────────
void Scores::_saveToFile() {
    std::ofstream f("Assets/Data/scores.txt");
    if (!f.is_open()) return;
    for (auto& e : _entries)
        f << e.name << " " << e.score << "\n";
}

// ── Chargement depuis le fichier texte ───────────────────────────────────────
void Scores::_loadFromFile() {
    std::ifstream f("Assets/Data/scores.txt");
    if (!f.is_open()) return;
    _entries.clear();
    std::string name;
    int score;
    while (f >> name >> score)
        _entries.push_back({name, score});
}

// ── Gestion des événements (retour au menu avec ESC) ─────────────────────────
void Scores::handleEvents(bool& back) {
    while (auto ev = _window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }
        if (auto* key = ev->getIf<sf::Event::KeyPressed>())
            if (key->code == sf::Keyboard::Key::Escape) back = true;
    }
}

// ── Rendu de l'écran des scores ───────────────────────────────────────────────
void Scores::render() {
    _window.clear(sf::Color(4, 4, 6));

    // Panneau principal
    sf::RectangleShape panel({540.f, 460.f});
    panel.setFillColor(sf::Color(5, 5, 8, 230));
    panel.setOutlineColor(sf::Color(220, 30, 30, 150));
    panel.setOutlineThickness(1.f);
    panel.setPosition({130.f, 70.f});
    _window.draw(panel);

    // Ligne rouge décorative en haut
    sf::RectangleShape topLine({540.f, 2.f});
    topLine.setFillColor(sf::Color(220, 30, 30));
    topLine.setPosition({130.f, 70.f});
    _window.draw(topLine);

    // Titre
    sf::Text title(_font);
    title.setCharacterSize(24);
    title.setFillColor(sf::Color(220, 30, 30));
    title.setLetterSpacing(3.f);
    title.setString("// MEILLEURS COMMANDANTS");
    title.setPosition({150.f, 88.f});
    _window.draw(title);

    // Séparateur
    sf::RectangleShape sep({500.f, 1.f});
    sep.setFillColor(sf::Color(220, 30, 30, 80));
    sep.setPosition({150.f, 125.f});
    _window.draw(sep);

    // Liste des scores
    if (_entries.empty()) {
        sf::Text empty(_font);
        empty.setCharacterSize(16);
        empty.setFillColor(sf::Color(80, 80, 80));
        empty.setString("// AUCUN ENREGISTREMENT");
        empty.setPosition({210.f, 270.f});
        _window.draw(empty);
    } else {
        for (int i = 0; i < (int)_entries.size(); ++i) {
            bool top3 = (i < 3);
            sf::Text line(_font);
            line.setCharacterSize(17);
            // Top 3 en rouge, reste en gris
            line.setFillColor(i == 0 ? sf::Color(220, 30, 30)
                            : i <= 2 ? sf::Color(200, 160, 60)
                                     : sf::Color(150, 150, 150));
            line.setString(std::to_string(i + 1) + ".   "
                         + _entries[i].name
                         + "   "
                         + std::to_string(_entries[i].score) + " pts");
            line.setPosition({150.f, 140.f + i * 32.f});
            _window.draw(line);
        }
    }

    // Retour
    sf::Text back(_font);
    back.setCharacterSize(13);
    back.setFillColor(sf::Color(80, 80, 80));
    back.setString("[ESC]  RETOUR");
    back.setPosition({150.f, 500.f});
    _window.draw(back);

    _window.display();
}
