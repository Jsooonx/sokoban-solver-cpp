#pragma once

#include "leaderboard.h"
#include "level.h"
#include "types.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class UI {
public:
    UI();

    void drawStartScreen(sf::RenderWindow& window, bool soundOn) const;
    void drawMainMenu(sf::RenderWindow& window, const std::vector<Level>& levels, int selectedLevel,
                      bool soundOn) const;
    void drawHud(sf::RenderWindow& window, const Level& level, int moves, int elapsedSeconds,
                 int hintsUsed, bool hintVisible, Direction nextHint, bool hasKey) const;
    void drawPause(sf::RenderWindow& window) const;
    void drawLevelComplete(sf::RenderWindow& window, const Level& level, int moves, int elapsedSeconds,
                           const Leaderboard& leaderboard) const;
    void drawGameComplete(sf::RenderWindow& window) const;
    void drawLeaderboard(sf::RenderWindow& window, const Level& level, const Leaderboard& leaderboard) const;
    void drawAbout(sf::RenderWindow& window) const;

    const sf::Font& getFont() const;
    bool hasFont() const;

private:
    sf::Font font;
    sf::Font titleFont;
    sf::Texture titleBannerTexture;
    sf::Texture frameTexture;
    sf::Texture buttonIdleTexture;
    sf::Texture buttonActiveTexture;
    sf::Texture slotTexture;
    sf::Texture checkIconTexture;
    sf::Texture crossIconTexture;
    sf::Texture keyTexture;

    bool fontLoaded = false;
    bool titleFontLoaded = false;
    bool titleBannerLoaded = false;
    bool frameTextureLoaded = false;
    bool buttonIdleTextureLoaded = false;
    bool buttonActiveTextureLoaded = false;
    bool slotTextureLoaded = false;
    bool checkIconTextureLoaded = false;
    bool crossIconTextureLoaded = false;
    bool keyTextureLoaded = false;

    void drawPanel(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size,
                   sf::Color fill = sf::Color(30, 37, 50, 236), sf::Color outline = sf::Color(71, 85, 105)) const;
    void drawNineSlice(sf::RenderWindow& window, const sf::Texture& texture, sf::Vector2f position, sf::Vector2f size, float border = 12.0f, float scale = 2.0f, sf::Color tint = sf::Color::White) const;

    void drawText(sf::RenderWindow& window, const std::string& text, unsigned size,
                  sf::Vector2f position, sf::Color color = sf::Color::White) const;
    void drawMainMenuTitle(sf::RenderWindow& window, float centerX, float y) const;
    void drawCenteredText(sf::RenderWindow& window, const std::string& text, unsigned size,
                          float centerX, float y, sf::Color color = sf::Color::White) const;
    void drawCenteredTextWithFont(sf::RenderWindow& window, const sf::Font& textFont,
                                  const std::string& text, unsigned size, float centerX,
                                  float y, sf::Color color = sf::Color::White) const;
};
