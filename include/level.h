#pragma once

#include "types.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Level {
public:
    static std::vector<Level> loadAll(const std::string& filename);

    bool isWall(int x, int y, bool keyCollected = true, bool doorOpen = true) const;
    bool isGoal(int x, int y) const;
    bool isInside(int x, int y) const;
    void render(sf::RenderWindow& window, float tileSize, sf::Vector2f offset, bool keyCollected = false, bool doorOpen = false) const;
    char getTileAt(int x, int y) const;

    int getId() const;
    int getWidth() const;
    int getHeight() const;
    int getDifficulty() const;
    int getOptimalMoves() const;
    const std::string& getName() const;
    const std::string& getSolution() const;
    Position getPlayerStart() const;
    const std::vector<Position>& getBoxStarts() const;
    const std::vector<Position>& getGoals() const;

private:
    int id = 0;
    int width = 0;
    int height = 0;
    int difficulty = 1;
    int optimalMoves = 0;
    std::string name;
    std::string solution;
    std::vector<std::string> grid;
    Position playerStart;
    std::vector<Position> boxStarts;
    std::vector<Position> goals;
};
