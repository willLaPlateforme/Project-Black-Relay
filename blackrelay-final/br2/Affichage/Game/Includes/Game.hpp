#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "../../../Game/Model/Includes/Observer.hpp"
#include "../../../Game/Model/Includes/Game.hpp"
#include "../../../Game/Model/Player_Tower/Includes/Player_Tower.hpp"

// ── GameView : Vue du jeu (pattern Observer) ─────────────────────────────────
// Reçoit les notifications du modèle Game et dessine tout l'écran de jeu
class GameView : public IObserver {
public:
    explicit GameView(sf::RenderWindow& window);

    // Appelé par le modèle quand une valeur change (Observer)
    void onEvent(const std::string& event, int value) override;

    // Boucle de rendu principale — appelée chaque frame
    void render(Game& game);

private:
    sf::RenderWindow&  _window;
    sf::Font           _font;
    sf::RectangleShape _hudBar;

    // Texture de la map en fond
    sf::Texture               _mapTexture;
    std::optional<sf::Sprite> _mapSprite;
    bool                      _hasMapBg = false;

    // Textes du HUD mis à jour via Observer
    std::optional<sf::Text> _scoreText;
    std::optional<sf::Text> _resourceText;
    std::optional<sf::Text> _baseHpText;
    std::optional<sf::Text> _waveText;
    std::optional<sf::Text> _towerInfoText;

    // ── Méthodes de dessin ────────────────────────────────────────────────────
    void _setupHUD();
    void _drawGame(Game& game);
    void _drawGrid();
    void _drawPath(const std::vector<sf::Vector2f>& path);
    void _drawSlots(const std::vector<Slot>& slots);
    void _drawSlotsOverlay(const std::vector<Slot>& slots);
    void _drawBlackRelay(const std::vector<sf::Vector2f>& path);
    void _drawHUD(Game& game);
    void _drawTowerMenu(Game& game);   // menu ingame de sélection des tours
    void _drawWaveTimer(Game& game);   // timer entre les vagues
    void _drawPause();
    void _drawEndScreen(bool victory);
};
