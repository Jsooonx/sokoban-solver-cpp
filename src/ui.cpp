#include "ui.h"
#include <algorithm>
#include <sstream>

#ifndef SOKOBAN_PROJECT_DIR
#define SOKOBAN_PROJECT_DIR "."
#endif

namespace {
const sf::Color Bg(21, 25, 34);
const sf::Color Panel(30, 37, 50, 236);
const sf::Color PanelSoft(38, 47, 64, 224);
const sf::Color Text(241, 245, 249);
const sf::Color Muted(156, 170, 190);
const sf::Color Accent(96, 165, 250);
const sf::Color Gold(245, 190, 72);
const sf::Color Green(74, 222, 128);
const sf::Color Danger(248, 113, 113);

std::string directionText(Direction direction) {
    switch (direction) {
        case Direction::Up: return "UP";
        case Direction::Down: return "DOWN";
        case Direction::Left: return "LEFT";
        case Direction::Right: return "RIGHT";
        case Direction::None: return "-";
    }
    return "-";
}

void drawBand(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size,
              sf::Color fill = PanelSoft, sf::Color outline = sf::Color(71, 85, 105, 160)) {
    sf::RectangleShape band(size);
    band.setPosition(position);
    band.setFillColor(fill);
    band.setOutlineThickness(1.0f);
    band.setOutlineColor(outline);
    window.draw(band);
}
}

UI::UI() {
    fontLoaded = font.loadFromFile(std::string(SOKOBAN_PROJECT_DIR) + "/assets/fonts/LowresPixel-Regular.otf")
        || font.loadFromFile("assets/fonts/LowresPixel-Regular.otf")
        || font.loadFromFile("../assets/fonts/LowresPixel-Regular.otf")
        || font.loadFromFile("/usr/share/fonts/truetype/ubuntu/UbuntuSans[wdth,wght].ttf")
        || font.loadFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf")
        || font.loadFromFile("assets/fonts/Inter-Regular.ttf")
        || font.loadFromFile("assets/fonts/arial.ttf")
        || font.loadFromFile("../assets/fonts/arial.ttf")
        || font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    titleFontLoaded = titleFont.loadFromFile(std::string(SOKOBAN_PROJECT_DIR) + "/assets/fonts/Pixel Game Extrude.otf")
        || titleFont.loadFromFile("assets/fonts/Pixel Game Extrude.otf")
        || titleFont.loadFromFile("../assets/fonts/Pixel Game Extrude.otf");
    titleBannerLoaded = titleBannerTexture.loadFromFile(std::string(SOKOBAN_PROJECT_DIR) + "/assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Banner01a.png")
        || titleBannerTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Banner01a.png")
        || titleBannerTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Banner01a.png");
    if (titleBannerLoaded) {
        titleBannerTexture.setSmooth(false);
    }

    std::string prefix = std::string(SOKOBAN_PROJECT_DIR) + "/assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/";
    frameTextureLoaded = frameTexture.loadFromFile(prefix + "UI_Flat_Frame01a.png")
        || frameTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Frame01a.png")
        || frameTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Frame01a.png");
    if (frameTextureLoaded) {
        frameTexture.setSmooth(false);
    }

    buttonIdleTextureLoaded = buttonIdleTexture.loadFromFile(prefix + "UI_Flat_Button01a_1.png")
        || buttonIdleTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Button01a_1.png")
        || buttonIdleTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Button01a_1.png");
    if (buttonIdleTextureLoaded) {
        buttonIdleTexture.setSmooth(false);
    }

    buttonActiveTextureLoaded = buttonActiveTexture.loadFromFile(prefix + "UI_Flat_Button01a_3.png")
        || buttonActiveTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Button01a_3.png")
        || buttonActiveTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Button01a_3.png");
    if (buttonActiveTextureLoaded) {
        buttonActiveTexture.setSmooth(false);
    }

    slotTextureLoaded = slotTexture.loadFromFile(prefix + "UI_Flat_FrameSlot01a.png")
        || slotTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_FrameSlot01a.png")
        || slotTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_FrameSlot01a.png");
    if (slotTextureLoaded) {
        slotTexture.setSmooth(false);
    }

    checkIconTextureLoaded = checkIconTexture.loadFromFile(prefix + "UI_Flat_IconCheck01a.png")
        || checkIconTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_IconCheck01a.png")
        || checkIconTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_IconCheck01a.png");
    if (checkIconTextureLoaded) {
        checkIconTexture.setSmooth(false);
    }

    crossIconTextureLoaded = crossIconTexture.loadFromFile(prefix + "UI_Flat_IconCross01a.png")
        || crossIconTexture.loadFromFile("assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_IconCross01a.png")
        || crossIconTexture.loadFromFile("../assets/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_IconCross01a.png");
    if (crossIconTextureLoaded) {
        crossIconTexture.setSmooth(false);
    }

    keyTextureLoaded = keyTexture.loadFromFile(std::string(SOKOBAN_PROJECT_DIR) + "/assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_tools_and_meterials.png")
        || keyTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_tools_and_meterials.png")
        || keyTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_tools_and_meterials.png")
        || keyTexture.loadFromFile(std::string(SOKOBAN_PROJECT_DIR) + "/assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic tools and meterials.png")
        || keyTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic tools and meterials.png")
        || keyTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic tools and meterials.png");
    if (keyTextureLoaded) {
        keyTexture.setSmooth(false);
    }
}

void UI::drawPanel(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2f size, sf::Color fill, sf::Color outline) const {
    // Drop shadow
    sf::RectangleShape shadow(size);
    shadow.setPosition(position + sf::Vector2f(0.0f, 6.0f));
    shadow.setFillColor(sf::Color(0, 0, 0, 72));
    window.draw(shadow);

    if (frameTextureLoaded) {
        // Tint the nine-slice frame with the fill color (dark panel)
        drawNineSlice(window, frameTexture, position, size, 12.0f, 2.0f, fill);
    } else {
        sf::RectangleShape panel(size);
        panel.setPosition(position);
        panel.setFillColor(fill);
        panel.setOutlineThickness(1.5f);
        panel.setOutlineColor(outline);
        window.draw(panel);
    }
}

void UI::drawNineSlice(sf::RenderWindow& window, const sf::Texture& texture, sf::Vector2f position, sf::Vector2f size, float border, float scale, sf::Color tint) const {
    sf::Vector2u texSize = texture.getSize();
    float B = border;
    float B_scaled = B * scale;

    if (size.x < 2.0f * B_scaled || size.y < 2.0f * B_scaled) {
        sf::Sprite sprite(texture);
        sprite.setColor(tint);
        sprite.setPosition(position);
        sprite.setScale(size.x / texSize.x, size.y / texSize.y);
        window.draw(sprite);
        return;
    }

    float w_inner = size.x - 2.0f * B_scaled;
    float h_inner = size.y - 2.0f * B_scaled;

    float W = static_cast<float>(texSize.x);
    float H = static_cast<float>(texSize.y);

    auto drawPatch = [&](sf::IntRect texRect, sf::Vector2f pos, sf::Vector2f patchSize) {
        sf::Sprite sprite(texture);
        sprite.setColor(tint);
        sprite.setTextureRect(texRect);
        sprite.setPosition(pos);
        sprite.setScale(patchSize.x / texRect.width, patchSize.y / texRect.height);
        window.draw(sprite);
    };

    // Corners
    drawPatch(sf::IntRect(0, 0, B, B), position, {B_scaled, B_scaled}); // TL
    drawPatch(sf::IntRect(W - B, 0, B, B), {position.x + size.x - B_scaled, position.y}, {B_scaled, B_scaled}); // TR
    drawPatch(sf::IntRect(0, H - B, B, B), {position.x, position.y + size.y - B_scaled}, {B_scaled, B_scaled}); // BL
    drawPatch(sf::IntRect(W - B, H - B, B, B), {position.x + size.x - B_scaled, position.y + size.y - B_scaled}, {B_scaled, B_scaled}); // BR

    // Edges
    drawPatch(sf::IntRect(B, 0, W - 2 * B, B), {position.x + B_scaled, position.y}, {w_inner, B_scaled}); // Top
    drawPatch(sf::IntRect(B, H - B, W - 2 * B, B), {position.x + B_scaled, position.y + size.y - B_scaled}, {w_inner, B_scaled}); // Bottom
    drawPatch(sf::IntRect(0, B, B, H - 2 * B), {position.x, position.y + B_scaled}, {B_scaled, h_inner}); // Left
    drawPatch(sf::IntRect(W - B, B, B, H - 2 * B), {position.x + size.x - B_scaled, position.y + B_scaled}, {B_scaled, h_inner}); // Right

    // Center
    drawPatch(sf::IntRect(B, B, W - 2 * B, H - 2 * B), {position.x + B_scaled, position.y + B_scaled}, {w_inner, h_inner}); // Center
}

void UI::drawText(sf::RenderWindow& window, const std::string& text, unsigned size, sf::Vector2f position, sf::Color color) const {
    if (!fontLoaded) {
        return;
    }
    sf::Text drawable(text, font, size);
    drawable.setFillColor(color);
    drawable.setPosition(position);
    window.draw(drawable);
}

void UI::drawMainMenuTitle(sf::RenderWindow& window, float centerX, float y) const {
    if (!fontLoaded) {
        return;
    }

    const std::string title = "SOKOBAN";
    if (titleBannerLoaded) {
        sf::Sprite banner(titleBannerTexture);
        const sf::Vector2u textureSize = titleBannerTexture.getSize();
        const float targetWidth = 244.0f;
        const float targetHeight = 76.0f;
        banner.setScale(targetWidth / static_cast<float>(textureSize.x),
                        targetHeight / static_cast<float>(textureSize.y));
        banner.setOrigin(static_cast<float>(textureSize.x) / 2.0f,
                         static_cast<float>(textureSize.y) / 2.0f);
        banner.setPosition(centerX, y + 28.0f);
        window.draw(banner);
    }

    if (titleFontLoaded) {
        sf::Text extrude(title, titleFont, 40);
        extrude.setFillColor(sf::Color(30, 41, 59));
        extrude.setLetterSpacing(1.22f);
        const sf::FloatRect extrudeBounds = extrude.getLocalBounds();
        extrude.setOrigin(extrudeBounds.left + extrudeBounds.width / 2.0f, extrudeBounds.top);
        extrude.setPosition(centerX + 3.0f, y + 23.0f);
        window.draw(extrude);
    }

    sf::Text face(title, font, 36);
    face.setFillColor(Text);
    face.setOutlineColor(sf::Color(15, 23, 42));
    face.setOutlineThickness(2.0f);
    face.setLetterSpacing(1.28f);
    const sf::FloatRect faceBounds = face.getLocalBounds();
    face.setOrigin(faceBounds.left + faceBounds.width / 2.0f, faceBounds.top);
    face.setPosition(centerX, y + 18.0f);
    window.draw(face);
}

void UI::drawCenteredText(sf::RenderWindow& window, const std::string& text, unsigned size, float centerX, float y, sf::Color color) const {
    if (!fontLoaded) {
        return;
    }
    drawCenteredTextWithFont(window, font, text, size, centerX, y, color);
}

void UI::drawCenteredTextWithFont(sf::RenderWindow& window, const sf::Font& textFont,
                                  const std::string& text, unsigned size, float centerX,
                                  float y, sf::Color color) const {
    sf::Text drawable(text, textFont, size);
    drawable.setFillColor(color);
    const sf::FloatRect bounds = drawable.getLocalBounds();
    drawable.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
    drawable.setPosition(centerX, y);
    window.draw(drawable);
}

void UI::drawStartScreen(sf::RenderWindow& window, bool soundOn) const {
    const float cx = window.getSize().x / 2.0f;
    drawPanel(window, {120.0f, 28.0f}, {660.0f, 642.0f}, sf::Color(25, 32, 45, 238), sf::Color(79, 96, 122));
    drawMainMenuTitle(window, cx, 100.0f);
    
    drawCenteredText(window, "Press ENTER or SPACE to start", 24, cx, 300.0f, Text);
    drawCenteredText(window, "Sound " + std::string(soundOn ? "On" : "Off"), 20, cx, 360.0f, soundOn ? Green : Danger);
    
    drawBand(window, {176.0f, 440.0f}, {548.0f, 120.0f}, sf::Color(20, 27, 39, 220), sf::Color(79, 96, 122));
    drawCenteredText(window, "Controls", 18, cx, 450.0f, Gold);
    drawCenteredText(window, "Arrows: Move    Z: Undo    R: Reset", 16, cx, 480.0f, sf::Color(200, 212, 228));
    drawCenteredText(window, "Space: Hint    H: Toggle Hint    Esc: Menu", 16, cx, 510.0f, sf::Color(200, 212, 228));
    
    drawCenteredText(window, "Press Q or Esc to Quit", 16, cx, 600.0f, Muted);
}

void UI::drawMainMenu(sf::RenderWindow& window, const std::vector<Level>& levels, int selectedLevel, bool soundOn) const {
    const float cx = window.getSize().x / 2.0f;
    drawPanel(window, {120.0f, 28.0f}, {660.0f, 642.0f}, sf::Color(25, 32, 45, 238), sf::Color(79, 96, 122));
    drawMainMenuTitle(window, cx, 56.0f);
    drawCenteredText(window, "Choose a level", 20, cx, 132.0f, Muted);
    drawBand(window, {176.0f, 166.0f}, {548.0f, 40.0f}, sf::Color(20, 27, 39, 220), sf::Color(79, 96, 122));
    drawCenteredText(window, "Enter Play    Up/Down Select    L Leaderboard    A About    S Sound", 16, cx, 177.0f, sf::Color(200, 212, 228));
    drawCenteredText(window, "Sound " + std::string(soundOn ? "On" : "Off"), 17, cx, 218.0f, soundOn ? Green : Danger);

    const float listTop = 258.0f;
    const int totalItems = static_cast<int>(levels.size()) + 1;
    const int visibleItems = std::min(totalItems, 10);
    const int maxFirstVisible = std::max(0, totalItems - visibleItems);
    const int firstVisible = std::clamp(selectedLevel - visibleItems + 1, 0, maxFirstVisible);
    const float rowGap = 35.5f;
    const float rowHeight = 31.0f;

    for (int visibleIndex = 0; visibleIndex < visibleItems; ++visibleIndex) {
        const int itemIndex = firstVisible + visibleIndex;
        const float y = listTop + static_cast<float>(visibleIndex) * rowGap;
        const bool selected = itemIndex == selectedLevel;
        if (buttonIdleTextureLoaded && buttonActiveTextureLoaded) {
            // Tint buttons dark so white text is readable
            sf::Color btnTint = selected ? sf::Color(67, 84, 120, 240) : sf::Color(28, 36, 52, 220);
            drawNineSlice(window, selected ? buttonActiveTexture : buttonIdleTexture, {182.0f, y - 7.0f}, {536.0f, rowHeight}, 10.0f, 1.0f, btnTint);
        } else {
            drawBand(window, {182.0f, y - 7.0f}, {536.0f, rowHeight},
                     selected ? sf::Color(67, 84, 112, 238) : sf::Color(31, 40, 55, 180),
                     selected ? Accent : sf::Color(66, 80, 101, 150));
        }
        if (itemIndex < static_cast<int>(levels.size())) {
            const Level& level = levels[itemIndex];
            drawText(window, selected ? ">" : "", 20, {204.0f, y - 3.0f}, Gold);
            drawText(window, level.getName(), 20, {236.0f, y - 3.0f}, selected ? Text : sf::Color(217, 226, 238));
            drawText(window, "Difficulty " + std::to_string(level.getDifficulty()), 16,
                     {590.0f, y}, selected ? Gold : Muted);
        } else {
            drawText(window, selected ? "> Exit" : "  Exit", 20, {204.0f, y - 3.0f},
                     selected ? Danger : Muted);
        }
    }

    if (totalItems > visibleItems) {
        const float trackHeight = static_cast<float>(visibleItems) * rowGap - 4.5f;
        const float trackX = 732.0f;
        const float trackY = listTop - 7.0f;
        sf::RectangleShape track({4.0f, trackHeight});
        track.setPosition(trackX, trackY);
        track.setFillColor(sf::Color(66, 80, 101, 130));
        window.draw(track);

        const float thumbHeight = std::max(34.0f, trackHeight * static_cast<float>(visibleItems) / static_cast<float>(totalItems));
        const float thumbTravel = trackHeight - thumbHeight;
        const float thumbY = maxFirstVisible == 0
            ? trackY
            : trackY + thumbTravel * static_cast<float>(firstVisible) / static_cast<float>(maxFirstVisible);
        sf::RectangleShape thumb({4.0f, thumbHeight});
        thumb.setPosition(trackX, thumbY);
        thumb.setFillColor(Accent);
        window.draw(thumb);
    }
}

void UI::drawHud(sf::RenderWindow& window, const Level& level, int moves, int elapsedSeconds, int hintsUsed, bool hintVisible, Direction nextHint, bool hasKey) const {
    // Panel now tall enough to contain the instruction line (y=8, h=80 => bottom at y=88)
    drawPanel(window, {18.0f, 8.0f}, {864.0f, 78.0f});
    drawText(window, level.getName(), 22, {34.0f, 18.0f}, Text);
    
    if (checkIconTextureLoaded && crossIconTextureLoaded) {
        bool isOptimal = moves <= level.getOptimalMoves();
        sf::Sprite icon(isOptimal ? checkIconTexture : crossIconTexture);
        // Tint icons with strong distinguishable colors
        icon.setColor(isOptimal ? sf::Color(74, 222, 128) : sf::Color(248, 113, 113));
        icon.setPosition(274.0f, 24.0f);
        sf::Vector2u iconSize = icon.getTexture()->getSize();
        icon.setScale(18.0f / iconSize.x, 18.0f / iconSize.y);
        window.draw(icon);
    }
    
    drawText(window, "Moves " + std::to_string(moves) + "/" + std::to_string(level.getOptimalMoves()), 18, {296.0f, 21.0f}, sf::Color(205, 216, 231));
    drawText(window, "Time " + std::to_string(elapsedSeconds) + "s", 18, {450.0f, 21.0f}, sf::Color(205, 216, 231));
    drawText(window, "Hints " + std::to_string(hintsUsed), 18, {565.0f, 21.0f}, sf::Color(205, 216, 231));
    
    if (slotTextureLoaded) {
        sf::Sprite slot(slotTexture);
        // Tint slot icon dark so it's distinguishable
        slot.setColor(sf::Color(120, 140, 180, 200));
        slot.setPosition(660.0f, 17.0f);
        window.draw(slot);
        
        if (hasKey && keyTextureLoaded) {
            sf::Sprite keySprite(keyTexture);
            keySprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
            keySprite.setPosition(666.0f, 23.0f);
            keySprite.setScale(1.5f, 1.5f);
            window.draw(keySprite);
        }
    } else if (hasKey) {
        drawText(window, "KEY", 18, {660.0f, 21.0f}, Gold);
    }
    
    if (hintVisible && nextHint != Direction::None) {
        drawText(window, "Hint " + directionText(nextHint), 19, {716.0f, 20.0f}, Gold);
    }
    // Instruction text inside the panel (below the HUD stats line)
    drawText(window, "Z Undo    R/Backspace Reset Level    Space Hint    H Toggle    Esc Menu", 14, {34.0f, 55.0f}, Muted);
}

void UI::drawPause(sf::RenderWindow& window) const {
    drawPanel(window, {(window.getSize().x - 520.0f) / 2.0f, 220.0f}, {520.0f, 196.0f},
              sf::Color(24, 31, 43, 242), sf::Color(96, 116, 145));
    const float cx = window.getSize().x / 2.0f;
    drawCenteredText(window, "PAUSED", 48, cx, 252.0f, Text);
    drawCenteredText(window, "Enter Resume    R/Backspace Reset Level    Esc Main Menu", 19, cx, 332.0f, Muted);
}

void UI::drawLevelComplete(sf::RenderWindow& window, const Level& level, int moves, int elapsedSeconds, const Leaderboard& leaderboard) const {
    drawPanel(window, {(window.getSize().x - 660.0f) / 2.0f, 104.0f}, {660.0f, 470.0f},
              sf::Color(24, 31, 43, 242), sf::Color(56, 189, 248));

    const float cx = window.getSize().x / 2.0f;
    drawCenteredText(window, "LEVEL COMPLETE", 42, cx, 134.0f, Gold);
    drawCenteredText(window, level.getName(), 23, cx, 194.0f, Text);
    drawBand(window, {246.0f, 242.0f}, {180.0f, 58.0f}, sf::Color(35, 44, 60, 232), sf::Color(71, 85, 105));
    drawBand(window, {474.0f, 242.0f}, {180.0f, 58.0f}, sf::Color(35, 44, 60, 232), sf::Color(71, 85, 105));
    drawCenteredText(window, "Moves", 15, 336.0f, 250.0f, Muted);
    drawCenteredText(window, std::to_string(moves), 25, 336.0f, 270.0f, Text);
    drawCenteredText(window, "Time", 15, 564.0f, 250.0f, Muted);
    drawCenteredText(window, std::to_string(elapsedSeconds) + "s", 25, 564.0f, 270.0f, Text);
    drawCenteredText(window, "Enter Next Level    R/Backspace Replay    L Leaderboard    Esc Menu", 16, cx, 322.0f, Muted);
    drawLeaderboard(window, level, leaderboard);
}

void UI::drawGameComplete(sf::RenderWindow& window) const {
    drawPanel(window, {(window.getSize().x - 620.0f) / 2.0f, 190.0f}, {620.0f, 300.0f},
              sf::Color(24, 31, 43, 242), Green);

    const float cx = window.getSize().x / 2.0f;
    drawCenteredText(window, "ALL LEVELS COMPLETE", 38, cx, 228.0f, Green);
    drawCenteredText(window, "Kamu sudah menyelesaikan semua level Sokoban.", 20, cx, 300.0f, Text);
    drawCenteredText(window, "Enter Main Menu    Q Exit", 20, cx, 370.0f, Gold);
}

void UI::drawLeaderboard(sf::RenderWindow& window, const Level& level, const Leaderboard& leaderboard) const {
    const float cx = window.getSize().x / 2.0f;
    drawCenteredText(window, "Leaderboard - " + level.getName(), 25, cx, 356.0f, Text);
    const auto scores = leaderboard.getTopScores(level.getName(), 5);
    if (scores.empty()) {
        drawCenteredText(window, "No scores yet.", 19, cx, 408.0f, Muted);
        return;
    }
    for (std::size_t i = 0; i < scores.size(); ++i) {
        const Score& score = scores[i];
        const float y = 402.0f + static_cast<float>(i) * 30.0f;
        drawBand(window, {228.0f, y - 5.0f}, {444.0f, 26.0f}, sf::Color(35, 44, 60, 190), sf::Color(66, 80, 101, 120));
        drawCenteredText(window, std::to_string(i + 1) + ". " + score.playerName + "  " + std::to_string(score.moves)
                 + " moves  " + std::to_string(score.time) + "s  " + score.date,
                 17, cx, y, sf::Color(224, 232, 244));
    }
}

void UI::drawAbout(sf::RenderWindow& window) const {
    const float cx = window.getSize().x / 2.0f;
    drawPanel(window, {110.0f, 92.0f}, {680.0f, 300.0f}, sf::Color(24, 31, 43, 240), sf::Color(79, 96, 122));
    drawCenteredText(window, "About", 42, cx, 126.0f, Text);
    drawCenteredText(window, "Sokoban klasik: dorong semua kotak ke goal.", 20, cx, 206.0f, sf::Color(224, 232, 244));
    drawCenteredText(window, "Kotak hanya bisa didorong, bukan ditarik.", 20, cx, 244.0f, sf::Color(224, 232, 244));
    drawCenteredText(window, "Dibuat dengan C++17 dan SFML.", 18, cx, 292.0f, Muted);
    drawCenteredText(window, "Esc Back", 22, cx, 344.0f, Gold);
}

const sf::Font& UI::getFont() const { return font; }
bool UI::hasFont() const { return fontLoaded; }
