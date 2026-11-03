#pragma once

#include "types.h"
#include <map>
#include <string>
#include <vector>

class Leaderboard {
public:
    explicit Leaderboard(std::string filePath);

    void loadFromFile();
    void saveToFile() const;
    void addScore(const std::string& levelName, int moves, int time, const std::string& playerName);
    std::vector<Score> getTopScores(const std::string& levelName, std::size_t limit) const;
    bool isNewRecord(const std::string& levelName, int moves, int time) const;

private:
    std::map<std::string, std::vector<Score>> data;
    std::string filePath;
};
