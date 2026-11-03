#pragma once

#include "types.h"
#include <SFML/Graphics.hpp>

class Box {
public:
    explicit Box(Position position);

    void moveTo(Position nextPosition);
    void render(sf::RenderWindow& window, float tileSize, sf::Vector2f offset, bool onGoal) const;

    Position getPosition() const;

private:
    Position position;
};
