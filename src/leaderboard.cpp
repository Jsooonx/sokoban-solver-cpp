#include "leaderboard.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <regex>

namespace {
std::string today() {
    std::time_t now = std::time(nullptr);
    char buffer[16] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&now));
    return buffer;
}
}

Leaderboard::Leaderboard(std::string filePath) : filePath(std::move(filePath)) {
    loadFromFile();
}

void Leaderboard::loadFromFile() {
    data.clear();
    std::ifstream input(filePath);
    if (!input) {
        return;
    }
    const std::string json((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const std::regex levelPattern("\"([^\"]+)\"\\s*:\\s*\\[([\\s\\S]*?)\\]", std::regex::ECMAScript);
    const std::regex scoreBlockPattern("\\{([^{}]+)\\}");
    const std::regex namePattern("\"playerName\"\\s*:\\s*\"([^\"]*)\"");
    const std::regex movesPattern("\"moves\"\\s*:\\s*(\\d+)");
    const std::regex timePattern("\"time\"\\s*:\\s*(\\d+)");
    const std::regex datePattern("\"date\"\\s*:\\s*\"([^\"]*)\"");
    
    for (auto lit = std::sregex_iterator(json.begin(), json.end(), levelPattern); lit != std::sregex_iterator(); ++lit) {
        const std::string levelName = (*lit)[1].str();
        const std::string scores = (*lit)[2].str();
        for (auto sit = std::sregex_iterator(scores.begin(), scores.end(), scoreBlockPattern); sit != std::sregex_iterator(); ++sit) {
            const std::string block = (*sit)[1].str();
            std::smatch match;
            std::string playerName;
            int moves = 0;
            int time = 0;
            std::string date;
            
            if (std::regex_search(block, match, namePattern)) {
                playerName = match[1].str();
            }
            if (std::regex_search(block, match, movesPattern)) {
                moves = std::stoi(match[1].str());
            }
            if (std::regex_search(block, match, timePattern)) {
                time = std::stoi(match[1].str());
            }
            if (std::regex_search(block, match, datePattern)) {
                date = match[1].str();
            }
            data[levelName].push_back({playerName, moves, time, date});
        }
    }
}

void Leaderboard::saveToFile() const {
    std::ofstream output(filePath);
    output << "{\n";
    std::size_t levelIndex = 0;
    for (const auto& [levelName, scores] : data) {
        output << "  \"" << levelName << "\": [\n";
        for (std::size_t i = 0; i < scores.size(); ++i) {
            const Score& score = scores[i];
            output << "    {\"playerName\": \"" << score.playerName << "\", \"moves\": " << score.moves
                   << ", \"time\": " << score.time << ", \"date\": \"" << score.date << "\"}";
            output << (i + 1 == scores.size() ? "\n" : ",\n");
        }
        output << "  ]" << (++levelIndex == data.size() ? "\n" : ",\n");
    }
    output << "}\n";
}

void Leaderboard::addScore(const std::string& levelName, int moves, int time, const std::string& playerName) {
    auto& scores = data[levelName];
    scores.push_back({playerName, moves, time, today()});
    std::sort(scores.begin(), scores.end(), [](const Score& a, const Score& b) {
        return a.moves == b.moves ? a.time < b.time : a.moves < b.moves;
    });
    if (scores.size() > 5) {
        scores.resize(5);
    }
    saveToFile();
}

std::vector<Score> Leaderboard::getTopScores(const std::string& levelName, std::size_t limit) const {
    auto found = data.find(levelName);
    if (found == data.end()) {
        return {};
    }
    std::vector<Score> scores = found->second;
    if (scores.size() > limit) {
        scores.resize(limit);
    }
    return scores;
}

bool Leaderboard::isNewRecord(const std::string& levelName, int moves, int time) const {
    const std::vector<Score> scores = getTopScores(levelName, 5);
    return scores.size() < 5 || moves < scores.back().moves || (moves == scores.back().moves && time < scores.back().time);
}
