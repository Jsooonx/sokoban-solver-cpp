#include "player.h"
#include <iostream>

namespace {
sf::Texture playerTexture;
bool textureLoaded = false;
sf::Clock globalClock;

void loadTextureIfNeeded() {
    if (!textureLoaded) {
        if (playerTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Basic Charakter Spritesheet.png") ||
            playerTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Basic Charakter Spritesheet.png")) {
            textureLoaded = true;
        } else {
            std::cerr << "Failed to load player texture\n";
        }
    }
}
}

namespace {
Position deltaFor(Direction direction) {
    switch (direction) {
        case Direction::Up: return {0, -1};
        case Direction::Down: return {0, 1};
        case Direction::Left: return {-1, 0};
        case Direction::Right: return {1, 0};
        case Direction::None: return {0, 0};
    }
    return {0, 0};
}
}

Player::Player(Position position) : position(position) {}

MoveRecord Player::tryMove(Direction direction, const Level& level, std::vector<Box>& boxes, bool keyCollected, bool doorOpen) {
    if (direction != Direction::None) {
        facingDirection = direction;
    }
    const Position delta = deltaFor(direction);
    MoveRecord record;
    record.playerBefore = position;
    record.playerAfter = position;

    const Position next{position.x + delta.x, position.y + delta.y};
    if (level.isWall(next.x, next.y, keyCollected, doorOpen)) {
        return record;
    }

    int boxIndex = -1;
    for (std::size_t i = 0; i < boxes.size(); ++i) {
        if (boxes[i].getPosition() == next) {
            boxIndex = static_cast<int>(i);
            break;
        }
    }

    if (boxIndex >= 0) {
        const Position pushed{next.x + delta.x, next.y + delta.y};
        if (level.isWall(pushed.x, pushed.y, keyCollected, doorOpen)) {
            return record;
        }
        for (const Box& box : boxes) {
            if (box.getPosition() == pushed) {
                return record;
            }
        }
        record.movedBoxIndex = boxIndex;
        record.boxBefore = next;
        record.boxAfter = pushed;
        boxes[boxIndex].moveTo(pushed);
    }

    position = next;
    lastMoveTimeMs = globalClock.getElapsedTime().asMilliseconds();
    record.playerAfter = position;
    return record;
}

void Player::setPosition(Position nextPosition) {
    position = nextPosition;
    lastMoveTimeMs = 0;
}

void Player::render(sf::RenderWindow& window, float tileSize, sf::Vector2f offset) const {
    loadTextureIfNeeded();
    if (textureLoaded) {
        sf::Sprite sprite(playerTexture);
        int row = 0;
        switch (facingDirection) {
            case Direction::Down: row = 0; break;
            case Direction::Up: row = 1; break;
            case Direction::Left: row = 2; break;
            case Direction::Right: row = 3; break;
            case Direction::None: row = 0; break;
        }
        int currentMs = globalClock.getElapsedTime().asMilliseconds();
        int timeSinceMove = currentMs - lastMoveTimeMs;
        int frame = 0;
        if (timeSinceMove >= 0 && timeSinceMove < 200) {
            frame = 2 + (currentMs / 100) % 2;
        } else {
            frame = (currentMs / 350) % 2;
        }
        sprite.setTextureRect(sf::IntRect(frame * 48, row * 48, 48, 48));
        
        float scale = tileSize / 24.0f;
        sprite.setScale(scale, scale);
        sprite.setOrigin(24.0f, 24.0f);
        sprite.setPosition(offset.x + position.x * tileSize + tileSize / 2.0f, 
                           offset.y + position.y * tileSize + tileSize / 2.0f - tileSize * 0.15f);
        window.draw(sprite);
    } else {
        sf::CircleShape body((tileSize - 10.0f) / 2.0f);
        body.setPosition(offset.x + position.x * tileSize + 5.0f, offset.y + position.y * tileSize + 5.0f);
        body.setFillColor(sf::Color(59, 130, 246));
        body.setOutlineThickness(3.0f);
        body.setOutlineColor(sf::Color(30, 64, 175));
        window.draw(body);
    }
}

Position Player::getPosition() const {
    return position;
}
