#pragma once

#include "box.h"
#include "level.h"
#include "types.h"
#include <string>
#include <vector>

#include <unordered_map>

class Solver {
public:
    void setSolution(const std::string& solution);
    Direction getMoveAt(std::size_t index) const;
    Direction getNextMove(const Level& level, Position player, const std::vector<Box>& boxes, bool keyCollected, bool doorOpen) const;
    const std::vector<Direction>& getSolution() const;

private:
    std::vector<Direction> moves;
    mutable std::unordered_map<uint64_t, Direction> memoCache;
};
