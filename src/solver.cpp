#include "solver.h"
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <string>
#include <vector>
#include <cstdint>

namespace {
Direction fromChar(char c) {
    switch (c) {
        case 'U': return Direction::Up;
        case 'D': return Direction::Down;
        case 'L': return Direction::Left;
        case 'R': return Direction::Right;
        default: return Direction::None;
    }
}

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

struct BoxesState {
    Position data[4];
    uint8_t size = 0;

    bool operator==(const BoxesState& other) const {
        if (size != other.size) return false;
        for (uint8_t i = 0; i < size; ++i) {
            if (!(data[i] == other.data[i])) return false;
        }
        return true;
    }
};

struct SearchState {
    Position player;
    BoxesState boxes;
    int parentIdx = -1;
    Direction moveFromParent = Direction::None;
    bool keyCollected = false;
    bool doorOpen = false;
};

BoxesState sortedBoxes(const std::vector<Box>& boxes) {
    BoxesState positions;
    positions.size = static_cast<uint8_t>(std::min(boxes.size(), std::size_t(4)));
    for (uint8_t i = 0; i < positions.size; ++i) {
        positions.data[i] = boxes[i].getPosition();
    }
    std::sort(positions.data, positions.data + positions.size);
    return positions;
}

uint64_t keyFor(Position player, const BoxesState& boxes, bool keyCollected) {
    uint64_t state = 0;
    state |= (keyCollected ? 1ULL : 0ULL);
    state |= (static_cast<uint64_t>(player.x) & 0x1FULL) << 1;
    state |= (static_cast<uint64_t>(player.y) & 0xFULL) << 6;
    for (std::size_t i = 0; i < 4; ++i) {
        uint64_t bx = 31;
        uint64_t by = 15;
        if (i < boxes.size) {
            bx = boxes.data[i].x;
            by = boxes.data[i].y;
        }
        std::size_t bitOffset = 10 + i * 9;
        state |= (bx & 0x1FULL) << bitOffset;
        state |= (by & 0xFULL) << (bitOffset + 5);
    }
    return state;
}

bool hasBox(const BoxesState& boxes, Position position, std::size_t* index = nullptr) {
    for (uint8_t i = 0; i < boxes.size; ++i) {
        if (boxes.data[i] == position) {
            if (index) {
                *index = i;
            }
            return true;
        }
    }
    return false;
}

bool complete(const Level& level, const BoxesState& boxes) {
    for (uint8_t i = 0; i < boxes.size; ++i) {
        if (!level.isGoal(boxes.data[i].x, boxes.data[i].y)) {
            return false;
        }
    }
    return boxes.size > 0;
}

Position findTile(const Level& level, char target) {
    for (int y = 0; y < level.getHeight(); ++y) {
        for (int x = 0; x < level.getWidth(); ++x) {
            if (level.getTileAt(x, y) == target) {
                return {x, y};
            }
        }
    }
    return {-1, -1};
}

Position computeIceSlide(const Level& level, Position start, Direction direction, int movedBoxIndex, const BoxesState& boxes, Position playerPos, bool keyCollected, bool doorOpen) {
    Position delta = deltaFor(direction);
    if (delta.x == 0 && delta.y == 0) {
        return start;
    }
    Position current = start;
    for (int i = 0; i < 2; ++i) {
        Position nextPos = {current.x + delta.x, current.y + delta.y};
        if (level.isWall(nextPos.x, nextPos.y, keyCollected, doorOpen)) {
            break;
        }
        bool boxCollision = false;
        for (uint8_t b = 0; b < boxes.size; ++b) {
            if (static_cast<int>(b) == movedBoxIndex) continue;
            if (boxes.data[b] == nextPos) {
                boxCollision = true;
                break;
            }
        }
        if (boxCollision) {
            break;
        }
        if (movedBoxIndex >= 0 && playerPos == nextPos) {
            break;
        }
        current = nextPos;
    }
    return current;
}

Position computeTeleport(const Level& level, Position start, int movedBoxIndex, const BoxesState& boxes, Position playerPos, Position portal1, Position portal2) {
    if (portal1.x == -1 || portal2.x == -1) {
        return start;
    }
    Position target = (start == portal1) ? portal2 : portal1;
    if (movedBoxIndex >= 0 && playerPos == target) {
        return start;
    }
    for (uint8_t b = 0; b < boxes.size; ++b) {
        if (static_cast<int>(b) == movedBoxIndex) continue;
        if (boxes.data[b] == target) {
            return start;
        }
    }
    return target;
}

class StateSet {
    std::vector<uint64_t> table;
    uint32_t mask;
public:
    StateSet(uint32_t capacity) {
        uint32_t cap = 1;
        while (cap < capacity) cap <<= 1;
        table.assign(cap, 0);
        mask = cap - 1;
    }

    bool insert(uint64_t key) {
        if (key == 0) key = 1;
        
        // Thomas Wang's 64-bit integer mix hash
        uint64_t hash = key;
        hash = (~hash) + (hash << 21);
        hash = hash ^ (hash >> 24);
        hash = (hash + (hash << 3)) + (hash << 8);
        hash = hash ^ (hash >> 14);
        hash = (hash + (hash << 2)) + (hash << 4);
        hash = hash ^ (hash >> 28);
        hash = hash + (hash << 31);

        uint32_t idx = static_cast<uint32_t>(hash) & mask;
        while (table[idx] != 0) {
            if (table[idx] == key) return false;
            idx = (idx + 1) & mask;
        }
        table[idx] = key;
        return true;
    }
};
}

void Solver::setSolution(const std::string& solution) {
    memoCache.clear();
    moves.clear();
    for (char c : solution) {
        const Direction direction = fromChar(c);
        if (direction != Direction::None) {
            moves.push_back(direction);
        }
    }
}

Direction Solver::getMoveAt(std::size_t index) const {
    return index < moves.size() ? moves[index] : Direction::None;
}

Direction Solver::getNextMove(const Level& level, Position player, const std::vector<Box>& boxes, bool keyCollected, bool doorOpen) const {
    const BoxesState startBoxes = sortedBoxes(boxes);
    const uint64_t startKey = keyFor(player, startBoxes, keyCollected);

    // 1. Check cache first
    auto it = memoCache.find(startKey);
    if (it != memoCache.end()) {
        return it->second;
    }

    // 2. Run BFS if not in cache
    const std::vector<Direction> directions = {
        Direction::Up,
        Direction::Down,
        Direction::Left,
        Direction::Right
    };

    Position keyPos = findTile(level, 'K');
    Position platePos = findTile(level, 'S');

    // Find portals
    Position portal1{-1, -1};
    Position portal2{-1, -1};
    for (int y = 0; y < level.getHeight(); ++y) {
        for (int x = 0; x < level.getWidth(); ++x) {
            if (level.getTileAt(x, y) == 'O') {
                if (portal1.x == -1) portal1 = {x, y};
                else portal2 = {x, y};
            }
        }
    }

    std::vector<SearchState> queue;
    queue.reserve(100000);

    StateSet visited(1048576); // Preallocate 1M entries for fast open-addressing
    SearchState start{player, startBoxes, -1, Direction::None, keyCollected, doorOpen};
    queue.push_back(start);
    visited.insert(startKey);

    int head = 0;
    constexpr int maxExploredStates = 500000;
    int goalIdx = -1;

    while (head < static_cast<int>(queue.size()) && head < maxExploredStates) {
        const int currentIdx = head++;
        const SearchState current = queue[currentIdx];

        if (complete(level, current.boxes)) {
            goalIdx = currentIdx;
            break;
        }

        for (Direction direction : directions) {
            const Position delta = deltaFor(direction);
            Position next{current.player.x + delta.x, current.player.y + delta.y};

            if (level.isWall(next.x, next.y, current.keyCollected, current.doorOpen)) {
                continue;
            }

            std::size_t pushedBoxIndex = 0;
            bool isPush = hasBox(current.boxes, next, &pushedBoxIndex);

            BoxesState nextBoxes = current.boxes;
            bool nextKeyCollected = current.keyCollected;

            if (isPush) {
                Position pushedBoxPos{next.x + delta.x, next.y + delta.y};
                if (level.isWall(pushedBoxPos.x, pushedBoxPos.y, current.keyCollected, current.doorOpen) || hasBox(current.boxes, pushedBoxPos)) {
                    continue;
                }

                // Box ice slide
                if (level.getTileAt(pushedBoxPos.x, pushedBoxPos.y) == 'I') {
                    pushedBoxPos = computeIceSlide(level, pushedBoxPos, direction, static_cast<int>(pushedBoxIndex), nextBoxes, next, current.keyCollected, current.doorOpen);
                }
                // Box teleport
                if (level.getTileAt(pushedBoxPos.x, pushedBoxPos.y) == 'O') {
                    pushedBoxPos = computeTeleport(level, pushedBoxPos, static_cast<int>(pushedBoxIndex), nextBoxes, next, portal1, portal2);
                }

                // Verify final box position is valid
                if (level.isWall(pushedBoxPos.x, pushedBoxPos.y, current.keyCollected, current.doorOpen) || hasBox(nextBoxes, pushedBoxPos)) {
                    continue;
                }

                nextBoxes.data[pushedBoxIndex] = pushedBoxPos;
                std::sort(nextBoxes.data, nextBoxes.data + nextBoxes.size);
            }

            Position nextPlayer = next;
            // Player ice slide
            if (level.getTileAt(nextPlayer.x, nextPlayer.y) == 'I') {
                nextPlayer = computeIceSlide(level, nextPlayer, direction, -1, nextBoxes, current.player, current.keyCollected, current.doorOpen);
            }
            // Player teleport
            if (level.getTileAt(nextPlayer.x, nextPlayer.y) == 'O') {
                nextPlayer = computeTeleport(level, nextPlayer, -1, nextBoxes, current.player, portal1, portal2);
            }

            // Key collection
            if (nextPlayer == keyPos) {
                nextKeyCollected = true;
            }

            // Door open state
            bool nextDoorOpen = false;
            if (platePos.x != -1) {
                if (nextPlayer == platePos) {
                    nextDoorOpen = true;
                } else {
                    for (uint8_t i = 0; i < nextBoxes.size; ++i) {
                        if (nextBoxes.data[i] == platePos) {
                            nextDoorOpen = true;
                            break;
                        }
                    }
                }
            }

            SearchState nextState;
            nextState.player = nextPlayer;
            nextState.boxes = nextBoxes;
            nextState.parentIdx = currentIdx;
            nextState.moveFromParent = direction;
            nextState.keyCollected = nextKeyCollected;
            nextState.doorOpen = nextDoorOpen;

            const uint64_t key = keyFor(nextState.player, nextState.boxes, nextState.keyCollected);
            if (visited.insert(key)) {
                queue.push_back(nextState);
            }
        }
    }

    // 3. Reconstruct path and cache it
    if (goalIdx != -1) {
        int idx = goalIdx;
        while (queue[idx].parentIdx != -1) {
            int pIdx = queue[idx].parentIdx;
            uint64_t pKey = keyFor(queue[pIdx].player, queue[pIdx].boxes, queue[pIdx].keyCollected);
            memoCache[pKey] = queue[idx].moveFromParent;
            idx = pIdx;
        }
        return memoCache[startKey];
    } else {
        memoCache[startKey] = Direction::None;
    }

    return Direction::None;
}

const std::vector<Direction>& Solver::getSolution() const {
    return moves;
}
