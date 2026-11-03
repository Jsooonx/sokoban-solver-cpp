#include "level.h"
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

#ifndef SOKOBAN_ASSET_DIR
#define SOKOBAN_ASSET_DIR "assets"
#endif

namespace {
struct TileTextures {
    sf::Texture floor;
    sf::Texture wall;
    sf::Texture ice;
    sf::Texture portal;
    sf::Texture key;
    sf::Texture gate;
    sf::Texture plate;
    sf::Texture door;
    sf::Texture goal;
    
    bool floorLoaded = false;
    bool wallLoaded = false;
    bool iceLoaded = false;
    bool portalLoaded = false;
    bool keyLoaded = false;
    bool gateLoaded = false;
    bool plateLoaded = false;
    bool doorLoaded = false;
    bool goalLoaded = false;
};

std::string readFile(const std::string& filename) {
    std::ifstream input(filename);
    if (!input) {
        throw std::runtime_error("Cannot open " + filename);
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string getString(const std::string& object, const std::string& key) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch match;
    return std::regex_search(object, match, pattern) ? match[1].str() : "";
}

int getInt(const std::string& object, const std::string& key, int fallback = 0) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    return std::regex_search(object, match, pattern) ? std::stoi(match[1].str()) : fallback;
}

std::vector<std::string> getStringArray(const std::string& object, const std::string& key) {
    std::vector<std::string> values;
    const std::regex arrayPattern("\"" + key + "\"\\s*:\\s*\\[([\\s\\S]*?)\\]");
    std::smatch arrayMatch;
    if (!std::regex_search(object, arrayMatch, arrayPattern)) {
        return values;
    }
    const std::string body = arrayMatch[1].str();
    const std::regex itemPattern("\"([^\"]*)\"");
    for (auto it = std::sregex_iterator(body.begin(), body.end(), itemPattern); it != std::sregex_iterator(); ++it) {
        values.push_back((*it)[1].str());
    }
    return values;
}

std::vector<Position> getPairs(const std::string& object, const std::string& key) {
    std::vector<Position> positions;
    const std::size_t keyPos = object.find("\"" + key + "\"");
    if (keyPos == std::string::npos) {
        return positions;
    }
    const std::size_t start = object.find('[', keyPos);
    if (start == std::string::npos) {
        return positions;
    }
    int depth = 0;
    std::size_t end = start;
    for (; end < object.size(); ++end) {
        if (object[end] == '[') {
            ++depth;
        } else if (object[end] == ']') {
            --depth;
            if (depth == 0) {
                break;
            }
        }
    }
    const std::string body = object.substr(start, end - start + 1);
    const std::regex pairPattern("\\[\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*\\]");
    for (auto it = std::sregex_iterator(body.begin(), body.end(), pairPattern); it != std::sregex_iterator(); ++it) {
        positions.push_back({std::stoi((*it)[1].str()), std::stoi((*it)[2].str())});
    }
    return positions;
}

std::vector<std::string> splitObjects(const std::string& json) {
    std::vector<std::string> objects;
    bool inString = false;
    int depth = 0;
    std::size_t start = std::string::npos;
    for (std::size_t i = 0; i < json.size(); ++i) {
        const char c = json[i];
        if (c == '"' && (i == 0 || json[i - 1] != '\\')) {
            inString = !inString;
        }
        if (inString) {
            continue;
        }
        if (c == '{') {
            if (depth == 1) {
                start = i;
            }
            ++depth;
        } else if (c == '}') {
            if (depth == 2 && start != std::string::npos) {
                objects.push_back(json.substr(start, i - start + 1));
                start = std::string::npos;
            }
            --depth;
        }
    }
    return objects;
}

bool loadFromAny(sf::Texture& texture, const std::vector<std::string>& paths) {
    for (const std::string& path : paths) {
        if (texture.loadFromFile(path)) {
            texture.setSmooth(false);
            return true;
        }
    }
    return false;
}

const TileTextures& tileTextures() {
    static TileTextures textures;
    static bool loaded = false;
    if (!loaded) {
        textures.floorLoaded = loadFromAny(textures.floor, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Grass.png",
            "textures/Sprout Lands - Sprites - Basic pack/Tilesets/Grass.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Tilesets/Grass.png",
            std::string(SOKOBAN_ASSET_DIR) + "/textures/floor.png",
            "textures/floor.png",
            "../textures/floor.png"
        });
        textures.wallLoaded = loadFromAny(textures.wall, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/wall.png",
            "textures/wall.png",
            "../textures/wall.png",
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Wooden_House_Walls_Tilset.png",
            "textures/Sprout Lands - Sprites - Basic pack/Tilesets/Wooden_House_Walls_Tilset.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Tilesets/Wooden_House_Walls_Tilset.png"
        });
        textures.iceLoaded = loadFromAny(textures.ice, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Water.png",
            "textures/Sprout Lands - Sprites - Basic pack/Tilesets/Water.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Tilesets/Water.png"
        });
        textures.portalLoaded = loadFromAny(textures.portal, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Select01a_3.png",
            "textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Select01a_3.png",
            "../textures/Complete_UI_Essential_Pack_Free/01_Flat_Theme/Sprites/UI_Flat_Select01a_3.png"
        });
        textures.keyLoaded = loadFromAny(textures.key, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_tools_and_meterials.png",
            "textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_tools_and_meterials.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_tools_and_meterials.png"
        });
        textures.gateLoaded = loadFromAny(textures.gate, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Fences.png",
            "textures/Sprout Lands - Sprites - Basic pack/Tilesets/Fences.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Tilesets/Fences.png"
        });
        textures.doorLoaded = loadFromAny(textures.door, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Doors.png",
            "textures/Sprout Lands - Sprites - Basic pack/Tilesets/Doors.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Tilesets/Doors.png"
        });
        textures.plateLoaded = loadFromAny(textures.plate, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Objects/Egg_item.png",
            "textures/Sprout Lands - Sprites - Basic pack/Objects/Egg_item.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Objects/Egg_item.png"
        });
        textures.goalLoaded = loadFromAny(textures.goal, {
            std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/Characters/Egg_And_Nest.png",
            "textures/Sprout Lands - Sprites - Basic pack/Characters/Egg_And_Nest.png",
            "../textures/Sprout Lands - Sprites - Basic pack/Characters/Egg_And_Nest.png"
        });
        loaded = true;
    }
    return textures;
}

void drawTexturedTile(sf::RenderWindow& window, const sf::Texture& texture, sf::Vector2f position, float tileSize, const sf::IntRect& textureRect = sf::IntRect(), const sf::Color& tintColor = sf::Color::White) {
    sf::Sprite sprite(texture);
    if (textureRect != sf::IntRect()) {
        sprite.setTextureRect(textureRect);
        sprite.setScale(
            tileSize / static_cast<float>(textureRect.width),
            tileSize / static_cast<float>(textureRect.height)
        );
    } else {
        sprite.setScale(
            tileSize / static_cast<float>(texture.getSize().x),
            tileSize / static_cast<float>(texture.getSize().y)
        );
    }
    sprite.setPosition(position);
    sprite.setColor(tintColor);
    window.draw(sprite);
}
}

std::vector<Level> Level::loadAll(const std::string& filename) {
    std::vector<Level> levels;
    for (const std::string& object : splitObjects(readFile(filename))) {
        if (object.find("\"grid\"") == std::string::npos) {
            continue;
        }
        Level level;
        level.id = getInt(object, "id");
        level.name = getString(object, "name");
        level.difficulty = getInt(object, "difficulty", 1);
        level.width = getInt(object, "width");
        level.height = getInt(object, "height");
        level.optimalMoves = getInt(object, "optimalMoves");
        level.solution = getString(object, "solution");
        level.grid = getStringArray(object, "grid");
        const std::vector<Position> starts = getPairs(object, "playerStart");
        level.playerStart = starts.empty() ? Position{} : starts.front();
        level.boxStarts = getPairs(object, "boxes");
        level.goals = getPairs(object, "goals");
        levels.push_back(level);
    }
    if (levels.empty()) {
        throw std::runtime_error("No levels found in " + filename);
    }
    return levels;
}

bool Level::isWall(int x, int y, bool keyCollected, bool doorOpen) const {
    if (!isInside(x, y)) {
        return true;
    }
    char tile = grid[y][x];
    if (tile == '#') {
        return true;
    }
    if (tile == 'L' && !keyCollected) {
        return true;
    }
    if (tile == 'D' && !doorOpen) {
        return true;
    }
    return false;
}

char Level::getTileAt(int x, int y) const {
    if (!isInside(x, y)) {
        return '#';
    }
    return grid[y][x];
}

bool Level::isGoal(int x, int y) const {
    for (const Position& goal : goals) {
        if (goal.x == x && goal.y == y) {
            return true;
        }
    }
    return false;
}

bool Level::isInside(int x, int y) const {
    return y >= 0 && y < static_cast<int>(grid.size()) && x >= 0 && x < static_cast<int>(grid[y].size());
}

void Level::render(sf::RenderWindow& window, float tileSize, sf::Vector2f offset, bool keyCollected, bool doorOpen) const {
    const TileTextures& textures = tileTextures();
    bool isSproutGrass = (textures.floorLoaded && textures.floor.getSize().x == 176 && textures.floor.getSize().y == 112);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const bool wallTile = isWall(x, y, keyCollected, doorOpen);
            const sf::Vector2f position(offset.x + x * tileSize, offset.y + y * tileSize);
            
            // Draw base tiles
            if (wallTile) {
                char tileChar = getTileAt(x, y);
                if (tileChar == 'L' || tileChar == 'D') {
                    if (isSproutGrass) {
                        drawTexturedTile(window, textures.floor, position, tileSize - 1.0f, sf::IntRect(16, 16, 16, 16));
                    } else if (textures.floorLoaded) {
                        drawTexturedTile(window, textures.floor, position, tileSize - 1.0f);
                    } else {
                        sf::RectangleShape tile({tileSize - 1.0f, tileSize - 1.0f});
                        tile.setPosition(position);
                        tile.setFillColor(sf::Color(241, 245, 249));
                        tile.setOutlineThickness(1.0f);
                        tile.setOutlineColor(sf::Color(203, 213, 225));
                        window.draw(tile);
                    }
                } else {
                    // Draw wall tile — use full texture directly (wall.png or fallback)
                    if (textures.wallLoaded) {
                        drawTexturedTile(window, textures.wall, position, tileSize - 1.0f);
                    } else {
                        sf::RectangleShape tile({tileSize - 1.0f, tileSize - 1.0f});
                        tile.setPosition(position);
                        tile.setFillColor(sf::Color(51, 65, 85));
                        tile.setOutlineThickness(1.0f);
                        tile.setOutlineColor(sf::Color(30, 41, 59));
                        window.draw(tile);
                    }
                }
            } else {
                if (isSproutGrass) {
                    int hash = (x * 73 + y * 37);
                    if (hash % 11 == 0) {
                        int detailIdx = (hash / 11) % 7;
                        sf::IntRect detailRect;
                        switch (detailIdx) {
                            case 0: detailRect = sf::IntRect(0, 80, 16, 16); break;
                            case 1: detailRect = sf::IntRect(48, 80, 16, 16); break;
                            case 2: detailRect = sf::IntRect(64, 80, 16, 16); break;
                            case 3: detailRect = sf::IntRect(80, 80, 16, 16); break;
                            case 4: detailRect = sf::IntRect(0, 96, 16, 16); break;
                            case 5: detailRect = sf::IntRect(48, 96, 16, 16); break;
                            case 6: detailRect = sf::IntRect(64, 96, 16, 16); break;
                        }
                        drawTexturedTile(window, textures.floor, position, tileSize - 1.0f, sf::IntRect(16, 16, 16, 16));
                        drawTexturedTile(window, textures.floor, position, tileSize - 1.0f, detailRect);
                    } else {
                        drawTexturedTile(window, textures.floor, position, tileSize - 1.0f, sf::IntRect(16, 16, 16, 16));
                    }
                } else if (textures.floorLoaded) {
                    drawTexturedTile(window, textures.floor, position, tileSize - 1.0f);
                } else {
                    sf::RectangleShape tile({tileSize - 1.0f, tileSize - 1.0f});
                    tile.setPosition(position);
                    tile.setFillColor(sf::Color(241, 245, 249));
                    tile.setOutlineThickness(1.0f);
                    tile.setOutlineColor(sf::Color(203, 213, 225));
                    window.draw(tile);
                }
            }
            
            // Draw interactive elements
            char tileChar = getTileAt(x, y);
            if (tileChar == 'I') { // Ice (Water)
                if (textures.iceLoaded) {
                    bool isSproutWater = (textures.ice.getSize().x == 64 && textures.ice.getSize().y == 16);
                    if (isSproutWater) {
                        static sf::Clock iceClock;
                        int frame = (iceClock.getElapsedTime().asMilliseconds() / 300) % 4;
                        drawTexturedTile(window, textures.ice, position, tileSize - 1.0f, sf::IntRect(frame * 16, 0, 16, 16), sf::Color(186, 230, 253, 220));
                    } else {
                        drawTexturedTile(window, textures.ice, position, tileSize - 1.0f, sf::IntRect(0, 0, 16, 16), sf::Color(186, 230, 253, 200));
                    }
                } else {
                    sf::RectangleShape ice({tileSize - 1.0f, tileSize - 1.0f});
                    ice.setPosition(position);
                    ice.setFillColor(sf::Color(14, 165, 233, 90));
                    ice.setOutlineThickness(1.0f);
                    ice.setOutlineColor(sf::Color(186, 230, 253, 120));
                    window.draw(ice);
                }
            } else if (tileChar == 'O') { // Portal
                if (textures.portalLoaded) {
                    sf::Sprite sprite(textures.portal);
                    sf::Vector2u size = textures.portal.getSize();
                    sprite.setTextureRect(sf::IntRect(0, 0, size.x, size.y));
                    sprite.setScale(tileSize / static_cast<float>(size.x), tileSize / static_cast<float>(size.y));
                    sprite.setOrigin(static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f);
                    sprite.setPosition(position.x + tileSize / 2.0f, position.y + tileSize / 2.0f);
                    
                    static sf::Clock portalClock;
                    float angle = portalClock.getElapsedTime().asSeconds() * 120.0f;
                    sprite.setRotation(angle);
                    
                    sprite.setColor(sf::Color(192, 132, 252));
                    window.draw(sprite);
                } else {
                    sf::CircleShape portal(tileSize * 0.35f);
                    portal.setPosition(offset.x + x * tileSize + tileSize * 0.15f, offset.y + y * tileSize + tileSize * 0.15f);
                    portal.setFillColor(sf::Color(147, 51, 234, 150));
                    portal.setOutlineThickness(3.0f);
                    portal.setOutlineColor(sf::Color(99, 102, 241));
                    window.draw(portal);
                }
            } else if (tileChar == 'K') { // Key
                if (!keyCollected) {
                    if (textures.keyLoaded) {
                        drawTexturedTile(window, textures.key, position, tileSize - 1.0f, sf::IntRect(0, 16, 16, 16));
                    } else {
                        sf::CircleShape keyShape(tileSize * 0.18f);
                        keyShape.setPosition(offset.x + x * tileSize + tileSize * 0.32f, offset.y + y * tileSize + tileSize * 0.32f);
                        keyShape.setFillColor(sf::Color(250, 204, 21));
                        keyShape.setOutlineThickness(2.0f);
                        keyShape.setOutlineColor(sf::Color(161, 98, 7));
                        window.draw(keyShape);
                    }
                }
            } else if (tileChar == 'L') { // Locked Gate
                if (textures.gateLoaded) {
                    if (!keyCollected) {
                        drawTexturedTile(window, textures.gate, position, tileSize - 1.0f, sf::IntRect(0, 0, 16, 16));
                    } else {
                        drawTexturedTile(window, textures.gate, position, tileSize - 1.0f, sf::IntRect(0, 0, 16, 16), sf::Color(255, 255, 255, 60));
                    }
                } else {
                    if (!keyCollected) {
                        sf::RectangleShape gate({tileSize * 0.8f, tileSize * 0.8f});
                        gate.setPosition(offset.x + x * tileSize + tileSize * 0.1f, offset.y + y * tileSize + tileSize * 0.1f);
                        gate.setFillColor(sf::Color(100, 116, 139));
                        gate.setOutlineThickness(2.0f);
                        gate.setOutlineColor(sf::Color(51, 65, 85));
                        window.draw(gate);
                    } else {
                        sf::RectangleShape gate({tileSize * 0.8f, tileSize * 0.8f});
                        gate.setPosition(offset.x + x * tileSize + tileSize * 0.1f, offset.y + y * tileSize + tileSize * 0.1f);
                        gate.setFillColor(sf::Color(100, 116, 139, 40));
                        gate.setOutlineThickness(1.0f);
                        gate.setOutlineColor(sf::Color(100, 116, 139, 80));
                        window.draw(gate);
                    }
                }
            } else if (tileChar == 'S') { // Pressure Plate
                if (textures.plateLoaded) {
                    sf::Color tint = doorOpen ? sf::Color(34, 197, 94) : sf::Color(239, 68, 68);
                    drawTexturedTile(window, textures.plate, position, tileSize - 1.0f, sf::IntRect(0, 0, 16, 16), tint);
                } else {
                    sf::RectangleShape plate({tileSize * 0.6f, tileSize * 0.6f});
                    plate.setPosition(offset.x + x * tileSize + tileSize * 0.2f, offset.y + y * tileSize + tileSize * 0.2f);
                    if (doorOpen) {
                        plate.setFillColor(sf::Color(34, 197, 94));
                        plate.setOutlineColor(sf::Color(22, 163, 74));
                    } else {
                        plate.setFillColor(sf::Color(239, 68, 68));
                        plate.setOutlineColor(sf::Color(220, 38, 38));
                    }
                    plate.setOutlineThickness(2.0f);
                    window.draw(plate);
                }
            } else if (tileChar == 'D') { // Door
                if (textures.doorLoaded) {
                    if (doorOpen) {
                        drawTexturedTile(window, textures.door, position, tileSize - 1.0f, sf::IntRect(0, 48, 16, 16), sf::Color(255, 255, 255, 100));
                    } else {
                        drawTexturedTile(window, textures.door, position, tileSize - 1.0f, sf::IntRect(0, 0, 16, 16));
                    }
                } else {
                    sf::RectangleShape door({tileSize - 4.0f, tileSize - 4.0f});
                    door.setPosition(offset.x + x * tileSize + 2.0f, offset.y + y * tileSize + 2.0f);
                    if (doorOpen) {
                        door.setFillColor(sf::Color(180, 83, 9, 40));
                        door.setOutlineColor(sf::Color(180, 83, 9, 80));
                    } else {
                        door.setFillColor(sf::Color(180, 83, 9));
                        door.setOutlineColor(sf::Color(120, 53, 4));
                    }
                    door.setOutlineThickness(2.0f);
                    window.draw(door);
                }
            }
            
            if (isGoal(x, y)) {
                if (textures.goalLoaded) {
                    drawTexturedTile(window, textures.goal, position, tileSize - 1.0f, sf::IntRect(0, 0, 16, 16));
                } else {
                    sf::CircleShape goal(tileSize * 0.24f);
                    goal.setPosition(offset.x + x * tileSize + tileSize * 0.26f, offset.y + y * tileSize + tileSize * 0.26f);
                    goal.setFillColor(sf::Color(239, 68, 68, 96));
                    goal.setOutlineThickness(3.0f);
                    goal.setOutlineColor(sf::Color(239, 68, 68));
                    window.draw(goal);
                }
            }
        }
    }
}

int Level::getId() const { return id; }
int Level::getWidth() const { return width; }
int Level::getHeight() const { return height; }
int Level::getDifficulty() const { return difficulty; }
int Level::getOptimalMoves() const { return optimalMoves; }
const std::string& Level::getName() const { return name; }
const std::string& Level::getSolution() const { return solution; }
Position Level::getPlayerStart() const { return playerStart; }
const std::vector<Position>& Level::getBoxStarts() const { return boxStarts; }
const std::vector<Position>& Level::getGoals() const { return goals; }
