#include "../Includes/Scores.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

Scores::Scores(sf::RenderWindow& window, sf::Font& font)
    : _window(&window), _font(font)
{
    // Charger les scores sauvegardés au démarrage
    loadFromFile();
}

void Scores::rebind(sf::RenderWindow& window) {
    _window = &window;
}

// ── Ajout d'un score et sauvegarde immédiate ──────────────────────────────────
void Scores::addScore(const std::string& name, int score) {
    if (score <= 0) return;

    _entries.push_back({name, score});

    // Trier par score décroissant
    std::sort(_entries.begin(), _entries.end(),
        [](const Entry& a, const Entry& b){ return a.score > b.score; });

    // Garder seulement les 10 meilleurs
    if ((int)_entries.size() > MAX_ENTRIES)
        _entries.resize(MAX_ENTRIES);

    // Sauvegarder immédiatement
    saveToFile();
}

// ── Sauvegarde dans Assets/Data/scores.json ───────────────────────────────────
void Scores::saveToFile(const std::string& path) {
    nlohmann::json j;
    j["scores"] = nlohmann::json::array();
    for (auto& e : _entries) {
        j["scores"].push_back({{"name", e.name}, {"score", e.score}});
    }
    std::ofstream f(path);
    if (f.is_open())
        f << j.dump(2);
}

// ── Chargement depuis Assets/Data/scores.json ─────────────────────────────────
void Scores::loadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return;
    try {
        auto j = nlohmann::json::parse(f);
        _entries.clear();
        for (auto& e : j["scores"]) {
            _entries.push_back({
                e.value("name", "CMD"),
                e.value("score", 0)
            });
        }
        // Trier par score décroissant
        std::sort(_entries.begin(), _entries.end(),
            [](const Entry& a, const Entry& b){ return a.score > b.score; });
    } catch (...) {
        // Fichier corrompu ou vide : on ignore
    }
}

// ── Gestion des événements ────────────────────────────────────────────────────
void Scores::handleEvents(bool& back) {
    while (auto ev = _window->pollEvent()) {
        if (ev->is<sf::Event::Closed>()) { back = true; return; }
        if (auto* k = ev->getIf<sf::Event::KeyPressed>())
            if (k->code == sf::Keyboard::Key::Escape) back = true;
        if (auto* click = ev->getIf<sf::Event::MouseButtonPressed>())
            if (click->button == sf::Mouse::Button::Left) {
                sf::FloatRect backBtn({300.f, 560.f}, {200.f, 40.f});
                if (backBtn.contains(sf::Vector2f(click->position)))
                    back = true;
            }
    }
}

// ── Rendu du tableau des scores ───────────────────────────────────────────────
void Scores::render() {
    _window->clear(sf::Color(4, 4, 6));

    // Scanlines
    for (int y = 0; y < 720; y += 4) {
        sf::RectangleShape line({1280.f, 1.f});
        line.setFillColor(sf::Color(0, 0, 0, 12));
        line.setPosition({0.f, (float)y});
        _window->draw(line);
    }

    // Titre
    sf::Text title(_font);
    title.setCharacterSize(28);
    title.setLetterSpacing(3.f);
    title.setFillColor(sf::Color(220, 30, 30));
    title.setString("// MEILLEURS SCORES");
    title.setPosition({390.f, 80.f});
    _window->draw(title);

    // Séparateur
    sf::RectangleShape sep({500.f, 1.f});
    sep.setFillColor(sf::Color(220, 30, 30, 80));
    sep.setPosition({390.f, 118.f});
    _window->draw(sep);

    if (_entries.empty()) {
        sf::Text none(_font);
        none.setCharacterSize(16);
        none.setFillColor(sf::Color(90, 90, 90));
        none.setString("Aucun score enregistre.\nTerminez une partie pour apparaitre ici !");
        none.setPosition({390.f, 200.f});
        _window->draw(none);
    }

    // En-tête colonnes
    sf::Text header(_font);
    header.setCharacterSize(13);
    header.setFillColor(sf::Color(140, 140, 140));
    header.setString("  #     COMMANDANT              SCORE");
    header.setPosition({390.f, 140.f});
    _window->draw(header);

    // Entrées
    for (int i = 0; i < (int)_entries.size(); ++i) {
        float y = 170.f + i * 34.f;

        // Fond alterné
        sf::RectangleShape bg({500.f, 30.f});
        bg.setFillColor(i == 0
            ? sf::Color(220, 30, 30, 30)  // 1er : rouge
            : sf::Color(255, 255, 255, i % 2 == 0 ? 5 : 0));
        bg.setPosition({385.f, y - 2.f});
        _window->draw(bg);

        // Rang
        sf::Text rank(_font);
        rank.setCharacterSize(15);
        rank.setFillColor(i == 0 ? sf::Color(220, 30, 30)
                        : i == 1 ? sf::Color(180, 180, 80)
                        : i == 2 ? sf::Color(140, 100, 60)
                                 : sf::Color(100, 100, 100));
        rank.setString(std::to_string(i + 1) + ".");
        rank.setPosition({390.f, y});
        _window->draw(rank);

        // Nom
        sf::Text name(_font);
        name.setCharacterSize(15);
        name.setFillColor(i == 0 ? sf::Color(255, 200, 200) : sf::Color(180, 180, 180));
        name.setString(_entries[i].name);
        name.setPosition({430.f, y});
        _window->draw(name);

        // Score
        sf::Text scoreText(_font);
        scoreText.setCharacterSize(15);
        scoreText.setFillColor(i == 0 ? sf::Color(220, 30, 30) : sf::Color(140, 200, 140));
        scoreText.setString(std::to_string(_entries[i].score) + " pts");
        scoreText.setPosition({680.f, y});
        _window->draw(scoreText);
    }

    // Bouton retour
    sf::FloatRect backRect({300.f, 560.f}, {200.f, 40.f});
    bool hover = backRect.contains(sf::Vector2f(sf::Mouse::getPosition(*_window)));
    sf::RectangleShape backBtn({200.f, 40.f});
    backBtn.setPosition({300.f, 560.f});
    backBtn.setFillColor(sf::Color(0, 0, 0, hover ? 200 : 150));
    backBtn.setOutlineColor(sf::Color(220, 30, 30, hover ? 180 : 80));
    backBtn.setOutlineThickness(1.f);
    _window->draw(backBtn);

    sf::Text backTxt(_font);
    backTxt.setCharacterSize(14);
    backTxt.setFillColor(sf::Color(220, 220, 220));
    backTxt.setString("[ESC] RETOUR");
    backTxt.setPosition({340.f, 572.f});
    _window->draw(backTxt);

    // Version
    sf::Text ver(_font);
    ver.setCharacterSize(10);
    ver.setFillColor(sf::Color(70, 70, 70));
    ver.setString("v1.0.0_BETA");
    ver.setPosition({1170.f, 703.f});
    _window->draw(ver);

    _window->display();
}