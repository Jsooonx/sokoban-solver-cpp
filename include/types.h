#pragma once

#include <string>

extern bool g_audioDeviceAvailable;

struct Position {
    int x = 0;
    int y = 0;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const Position& other) const {
        return y == other.y ? x < other.x : y < other.y;
    }
};

enum class Direction {
    Up,
    Down,
    Left,
    Right,
    None
};

struct MoveRecord {
    Position playerBefore;
    Position playerAfter;
    int movedBoxIndex = -1;
    Position boxBefore;
    Position boxAfter;
    bool keyCollectedBefore = false;
    bool keyCollectedAfter = false;
    bool doorOpenBefore = false;
    bool doorOpenAfter = false;
};

struct Score {
    std::string playerName;
    int moves = 0;
    int time = 0;
    std::string date;
};
