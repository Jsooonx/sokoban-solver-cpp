#include "box.h"
#include <string>

#ifndef SOKOBAN_ASSET_DIR
#define SOKOBAN_ASSET_DIR "assets"
#endif

namespace {
const sf::Texture* chestTexture() {
    static sf::Texture texture;
    static const bool loaded = [] {
        const bool ok = texture.loadFromFile(std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Objects/Chest.png")
            || texture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Chest.png")
            || texture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Chest.png")
            || texture.loadFromFile(std::string(SOKOBAN_ASSET_DIR) + "/textures/Chest.png")
            || texture.loadFromFile("assets/textures/Chest.png")
            || texture.loadFromFile("../assets/textures/Chest.png");
        if (ok) {
            texture.setSmooth(false);
        }
        return ok;
    }();
    return loaded ? &texture : nullptr;
}

const sf::Texture* nestTexture() {
    static sf::Texture texture;
    static const bool loaded = [] {
        const bool ok = texture.loadFromFile(std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Characters/Egg_And_Nest.png")
            || texture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Egg_And_Nest.png")
            || texture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Egg_And_Nest.png");
        if (ok) {
            texture.setSmooth(false);
        }
        return ok;
    }();
    return loaded ? &texture : nullptr;
}
}

Box::Box(Position position) : position(position) {}

void Box::moveTo(Position nextPosition) {
    position = nextPosition;
}

void Box::render(sf::RenderWindow& window, float tileSize, sf::Vector2f offset, bool onGoal) const {
    const sf::Texture* chestTex = chestTexture();
    if (chestTex) {
        bool isSproutLands = (chestTex->getSize().x == 240 && chestTex->getSize().y == 96);
        if (isSproutLands) {
            if (onGoal) {
                const sf::Texture* nestTex = nestTexture();
                if (nestTex) {
                    sf::Sprite nest(*nestTex);
                    nest.setTextureRect(sf::IntRect(48, 0, 16, 16));
                    nest.setScale(tileSize / 16.0f, tileSize / 16.0f);
                    nest.setPosition(offset.x + position.x * tileSize, offset.y + position.y * tileSize);
                    window.draw(nest);
                }
            }

            sf::Sprite chest(*chestTex);
            sf::IntRect chestRect = onGoal ? sf::IntRect(192, 0, 48, 48) : sf::IntRect(0, 0, 48, 48);
            chest.setTextureRect(chestRect);
            // Scale chest to 1.35x tileSize so it's prominently visible
            float chestScale = (tileSize * 1.35f) / 48.0f;
            chest.setScale(chestScale, chestScale);
            // Center on tile (offset by half the overflow so it stays centered)
            float overflow = (tileSize * 1.35f - tileSize) / 2.0f;
            chest.setPosition(offset.x + position.x * tileSize - overflow,
                              offset.y + position.y * tileSize - overflow);
            window.draw(chest);
            return;
        } else {
            sf::Sprite chest(*chestTex);
            const sf::Vector2u textureSize = chestTex->getSize();
            const float targetSize = tileSize - 6.0f;
            const float scale = targetSize / static_cast<float>(std::max(textureSize.x, textureSize.y));
            chest.setScale(scale, scale);
            const sf::FloatRect bounds = chest.getLocalBounds();
            chest.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
            chest.setPosition(offset.x + position.x * tileSize + tileSize / 2.0f,
                              offset.y + position.y * tileSize + tileSize / 2.0f);
            chest.setColor(onGoal ? sf::Color(210, 255, 220) : sf::Color::White);
            window.draw(chest);
            return;
        }
    }

    sf::RectangleShape box({tileSize - 8.0f, tileSize - 8.0f});
    box.setPosition(offset.x + position.x * tileSize + 4.0f, offset.y + position.y * tileSize + 4.0f);
    box.setFillColor(onGoal ? sf::Color(34, 197, 94) : sf::Color(245, 158, 11));
    box.setOutlineThickness(3.0f);
    box.setOutlineColor(onGoal ? sf::Color(21, 128, 61) : sf::Color(146, 64, 14));
    window.draw(box);
}

Position Box::getPosition() const {
    return position;
}
