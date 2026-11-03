#pragma once

#include "box.h"
#include "level.h"
#include "types.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Player {
public:
    explicit Player(Position position = {});

    MoveRecord tryMove(Direction direction, const Level& level, std::vector<Box>& boxes, bool keyCollected, bool doorOpen);
    void setPosition(Position nextPosition);
    void render(sf::RenderWindow& window, float tileSize, sf::Vector2f offset) const;
    Position getPosition() const;

private:
    Position position;
    Direction facingDirection = Direction::Down;
    int lastMoveTimeMs = 0;
};
