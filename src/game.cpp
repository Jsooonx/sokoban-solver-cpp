#include "game.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <ctime>

#ifndef SOKOBAN_ASSET_DIR
#define SOKOBAN_ASSET_DIR "assets"
#endif

#ifndef SOKOBAN_DATA_DIR
#define SOKOBAN_DATA_DIR "data"
#endif

namespace {
bool moved(const MoveRecord& record) {
    return !(record.playerBefore == record.playerAfter);
}

int exitMenuIndex(const std::vector<Level>& levels) {
    return static_cast<int>(levels.size());
}
}

Game::Game()
    : window(sf::VideoMode(900, 720), "Sokoban"),
      leaderboard(std::string(SOKOBAN_DATA_DIR) + "/leaderboard.json") {
    window.setFramerateLimit(60);
    if (backgroundMusic.openFromFile(std::string(SOKOBAN_ASSET_DIR) + "/song/retrosunrise-popcorn-and-videogames-233568.mp3")) {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(45.0f);
        setSoundEnabled(soundOn);
    }
    levels = Level::loadAll(std::string(SOKOBAN_ASSET_DIR) + "/levels/levels.json");

    // Load scenery textures
    std::string prefix = std::string(SOKOBAN_ASSET_DIR) + "/textures/Sprout Lands - Sprites - Basic pack/";
    
    cowTextureLoaded = cowTexture.loadFromFile(prefix + "Characters/Free Cow Sprites.png")
        || cowTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Free Cow Sprites.png")
        || cowTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Free Cow Sprites.png");
    if (cowTextureLoaded) cowTexture.setSmooth(false);

    chickenTextureLoaded = chickenTexture.loadFromFile(prefix + "Characters/Free Chicken Sprites.png")
        || chickenTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Free Chicken Sprites.png")
        || chickenTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Characters/Free Chicken Sprites.png");
    if (chickenTextureLoaded) chickenTexture.setSmooth(false);

    grassBgTextureLoaded = grassBgTexture.loadFromFile(prefix + "Tilesets/Grass.png")
        || grassBgTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Grass.png")
        || grassBgTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Grass.png");
    if (grassBgTextureLoaded) grassBgTexture.setSmooth(false);

    fenceTextureLoaded = fenceTexture.loadFromFile(prefix + "Tilesets/Fences.png")
        || fenceTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Fences.png")
        || fenceTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Tilesets/Fences.png");
    if (fenceTextureLoaded) fenceTexture.setSmooth(false);

    decoTextureLoaded = decoTexture.loadFromFile(prefix + "Objects/Basic Grass Biom things 1.png")
        || decoTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic Grass Biom things 1.png")
        || decoTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic Grass Biom things 1.png")
        || decoTexture.loadFromFile(prefix + "Objects/Basic_Grass_Biom_things.png")
        || decoTexture.loadFromFile("assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_Grass_Biom_things.png")
        || decoTexture.loadFromFile("../assets/textures/Sprout Lands - Sprites - Basic pack/Objects/Basic_Grass_Biom_things.png");
    if (decoTextureLoaded) decoTexture.setSmooth(false);

    srand(static_cast<unsigned>(time(nullptr)));

    loadLevel(0);
    state = GameState::StartScreen;
    updateClock.restart();
}

void Game::run() {
    while (window.isOpen()) {
        handleInput();
        update();
        render();
    }
}

void Game::handleInput() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type != sf::Event::KeyPressed) {
            continue;
        }

        const sf::Keyboard::Key key = event.key.code;
        if (state == GameState::StartScreen) {
            if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) state = GameState::MainMenu;
            if (key == sf::Keyboard::S) setSoundEnabled(!soundOn);
            if (key == sf::Keyboard::Q || key == sf::Keyboard::Escape) window.close();
        } else if (state == GameState::MainMenu) {
            const int menuItems = static_cast<int>(levels.size()) + 1;
            if (key == sf::Keyboard::Up) currentLevelIndex = (currentLevelIndex + menuItems - 1) % menuItems;
            if (key == sf::Keyboard::Down) currentLevelIndex = (currentLevelIndex + 1) % menuItems;
            if (key == sf::Keyboard::Enter) {
                if (currentLevelIndex == exitMenuIndex(levels)) {
                    window.close();
                } else {
                    loadLevel(currentLevelIndex);
                }
            }
            if (key == sf::Keyboard::L) state = GameState::Leaderboard;
            if (key == sf::Keyboard::A) state = GameState::About;
            if (key == sf::Keyboard::Q) window.close();
            if (key == sf::Keyboard::S) setSoundEnabled(!soundOn);
        } else if (state == GameState::Playing) {
            if (key == sf::Keyboard::Escape) state = GameState::MainMenu;
            if (key == sf::Keyboard::P) state = GameState::Paused;
            if (key == sf::Keyboard::R || key == sf::Keyboard::BackSpace) restartLevel();
            if (key == sf::Keyboard::Z && !undoStack.empty()) {
                const MoveRecord record = undoStack.back();
                undoStack.pop_back();
                player.setPosition(record.playerBefore);
                if (record.movedBoxIndex >= 0 && record.movedBoxIndex < static_cast<int>(boxes.size())) {
                    boxes[record.movedBoxIndex].moveTo(record.boxBefore);
                }
                keyCollected = record.keyCollectedBefore;
                doorOpen = record.doorOpenBefore;
                moveCount = std::max(0, moveCount - 1);
                refreshHint();
            }
            if (key == sf::Keyboard::H) hintVisible = !hintVisible;
            if (key == sf::Keyboard::Space) {
                refreshHint();
                hintVisible = true;
                if (activeHint != Direction::None) {
                    ++hintsUsed;
                }
            }
            if (key == sf::Keyboard::Up) movePlayer(Direction::Up);
            if (key == sf::Keyboard::Down) movePlayer(Direction::Down);
            if (key == sf::Keyboard::Left) movePlayer(Direction::Left);
            if (key == sf::Keyboard::Right) movePlayer(Direction::Right);
        } else if (state == GameState::Paused) {
            if (key == sf::Keyboard::Enter || key == sf::Keyboard::P) state = GameState::Playing;
            if (key == sf::Keyboard::R || key == sf::Keyboard::BackSpace) restartLevel();
            if (key == sf::Keyboard::Escape) state = GameState::MainMenu;
        } else if (state == GameState::LevelComplete) {
            if (key == sf::Keyboard::Enter) {
                if (currentLevelIndex + 1 >= static_cast<int>(levels.size())) {
                    state = GameState::GameComplete;
                } else {
                    loadLevel(currentLevelIndex + 1);
                }
            }
            if (key == sf::Keyboard::R || key == sf::Keyboard::BackSpace) restartLevel();
            if (key == sf::Keyboard::L) state = GameState::Leaderboard;
            if (key == sf::Keyboard::Escape) state = GameState::MainMenu;
        } else if (state == GameState::GameComplete) {
            if (key == sf::Keyboard::Enter || key == sf::Keyboard::Escape) state = GameState::MainMenu;
            if (key == sf::Keyboard::Q) window.close();
        } else {
            if (key == sf::Keyboard::Escape) state = GameState::MainMenu;
            if (state == GameState::Leaderboard && key == sf::Keyboard::Up) currentLevelIndex = (currentLevelIndex + static_cast<int>(levels.size()) - 1) % static_cast<int>(levels.size());
            if (state == GameState::Leaderboard && key == sf::Keyboard::Down) currentLevelIndex = (currentLevelIndex + 1) % static_cast<int>(levels.size());
        }
    }
}

void Game::update() {
    float dt = std::min(updateClock.restart().asSeconds(), 0.1f);
    
    if (state == GameState::Playing && isComplete()) {
        completionTime = static_cast<int>(levelClock.getElapsedTime().asSeconds());
        saveBestScore(completionTime);
        state = GameState::LevelComplete;
    }
}

void Game::render() {
    window.clear(sf::Color(31, 34, 40));

    if (state == GameState::StartScreen) {
        ui.drawStartScreen(window, soundOn);
    } else if (state == GameState::MainMenu) {
        ui.drawMainMenu(window, levels, currentLevelIndex, soundOn);
    } else if (state == GameState::About) {
        ui.drawAbout(window);
    } else if (state == GameState::Leaderboard) {
        ui.drawLeaderboard(window, levels[currentLevelIndex], leaderboard);
        if (ui.hasFont()) {
            sf::Text hint("Up/Down: Change level    Esc: Menu", ui.getFont(), 18);
            hint.setPosition(245.0f, 620.0f);
            window.draw(hint);
        }
    } else if (state == GameState::GameComplete) {
        ui.drawGameComplete(window);
    } else {
        const Level& level = levels[currentLevelIndex];
        const float size = tileSize();
        const sf::Vector2f offset = boardOffset(size);

        // Render grass background
        if (grassBgTextureLoaded) {
            sf::Sprite grass(grassBgTexture);
            grass.setTextureRect(sf::IntRect(16, 16, 16, 16));
            grass.setScale(2.0f, 2.0f);
            grass.setColor(sf::Color(180, 210, 180));
            for (float y = 0.0f; y < 720.0f; y += 32.0f) {
                for (float x = 0.0f; x < 900.0f; x += 32.0f) {
                    grass.setPosition(x, y);
                    window.draw(grass);
                }
            }
        }

        // Render scenery decorations
        if (decoTextureLoaded) {
            sf::Sprite deco(decoTexture);
            for (const SceneryDeco& d : sceneryDecorations) {
                deco.setTextureRect(d.rect);
                deco.setScale(d.scale, d.scale);
                deco.setPosition(d.position);
                window.draw(deco);
            }
        }

        // Render fence border enclosing the level board
        if (fenceTextureLoaded) {
            sf::Sprite fence(fenceTexture);
            fence.setTextureRect(sf::IntRect(16, 0, 16, 16));
            float fenceScale = size / 16.0f;
            fence.setScale(fenceScale, fenceScale);
            for (int gx = -1; gx <= level.getWidth(); ++gx) {
                fence.setPosition(offset.x + gx * size, offset.y - size);
                window.draw(fence);
                fence.setPosition(offset.x + gx * size, offset.y + level.getHeight() * size);
                window.draw(fence);
            }
            for (int gy = 0; gy < level.getHeight(); ++gy) {
                fence.setPosition(offset.x - size, offset.y + gy * size);
                window.draw(fence);
                fence.setPosition(offset.x + level.getWidth() * size, offset.y + gy * size);
                window.draw(fence);
            }
        }



        level.render(window, size, offset, keyCollected, doorOpen);
        for (const Box& box : boxes) {
            const Position position = box.getPosition();
            box.render(window, size, offset, level.isGoal(position.x, position.y));
        }
        player.render(window, size, offset);
        if (state == GameState::Playing && hintVisible && activeHint != Direction::None) {
            drawHintArrow(size, offset);
        }

        const int elapsed = state == GameState::LevelComplete
            ? completionTime
            : static_cast<int>(levelClock.getElapsedTime().asSeconds());
        if (state == GameState::Playing) {
            ui.drawHud(window, level, moveCount, elapsed, hintsUsed, hintVisible, activeHint, keyCollected);
        } else if (state == GameState::Paused) {
            ui.drawPause(window);
        } else if (state == GameState::LevelComplete) {
            ui.drawLevelComplete(window, level, moveCount, elapsed, leaderboard);
        }
    }

    window.display();
}

void Game::loadLevel(int index) {
    currentLevelIndex = std::clamp(index, 0, static_cast<int>(levels.size()) - 1);
    const Level& level = levels[currentLevelIndex];
    player = Player(level.getPlayerStart());
    boxes.clear();
    for (const Position& position : level.getBoxStarts()) {
        boxes.emplace_back(position);
    }
    solver.setSolution(level.getSolution());
    undoStack.clear();
    moveCount = 0;
    hintsUsed = 0;
    completionTime = 0;
    scoreSaved = false;
    keyCollected = false;
    doorOpen = false;
    cacheLevelElements();
    updatePlateTrigger();
    levelClock.restart();
    refreshHint();
    initScenery();
    state = GameState::Playing;
}

void Game::restartLevel() {
    loadLevel(currentLevelIndex);
}

void Game::setSoundEnabled(bool enabled) {
    soundOn = enabled;
    if (soundOn && g_audioDeviceAvailable) {
        if (backgroundMusic.getStatus() != sf::Music::Playing) {
            backgroundMusic.play();
            if (!g_audioDeviceAvailable) {
                backgroundMusic.stop();
            }
        }
    } else {
        backgroundMusic.pause();
    }
}

void Game::movePlayer(Direction direction) {
    bool keyCollectedBefore = keyCollected;
    bool doorOpenBefore = doorOpen;

    MoveRecord record = player.tryMove(direction, levels[currentLevelIndex], boxes, keyCollected, doorOpen);
    if (!moved(record)) {
        return;
    }

    // Player slides on Ice
    Position playerPos = player.getPosition();
    if (levels[currentLevelIndex].getTileAt(playerPos.x, playerPos.y) == 'I') {
        playerPos = computeIceSlide(playerPos, direction, -1);
        player.setPosition(playerPos);
    }
    // Player teleports via Portal
    if (levels[currentLevelIndex].getTileAt(playerPos.x, playerPos.y) == 'O') {
        playerPos = computeTeleport(playerPos, -1);
        player.setPosition(playerPos);
    }

    // Box slides and teleports
    if (record.movedBoxIndex >= 0) {
        Position boxPos = boxes[record.movedBoxIndex].getPosition();
        if (levels[currentLevelIndex].getTileAt(boxPos.x, boxPos.y) == 'I') {
            boxPos = computeIceSlide(boxPos, direction, record.movedBoxIndex);
            boxes[record.movedBoxIndex].moveTo(boxPos);
        }
        if (levels[currentLevelIndex].getTileAt(boxPos.x, boxPos.y) == 'O') {
            boxPos = computeTeleport(boxPos, record.movedBoxIndex);
            boxes[record.movedBoxIndex].moveTo(boxPos);
        }
    }

    // Check key collection
    if (player.getPosition() == keyPos && !keyCollected) {
        keyCollected = true;
    }

    // Update plates and door states
    updatePlateTrigger();

    // Populate record metadata
    record.playerAfter = player.getPosition();
    if (record.movedBoxIndex >= 0) {
        record.boxAfter = boxes[record.movedBoxIndex].getPosition();
    }
    record.keyCollectedBefore = keyCollectedBefore;
    record.keyCollectedAfter = keyCollected;
    record.doorOpenBefore = doorOpenBefore;
    record.doorOpenAfter = doorOpen;

    undoStack.push_back(record);
    if (undoStack.size() > 50) {
        undoStack.pop_front();
    }
    ++moveCount;
    refreshHint();
}

void Game::cacheLevelElements() {
    portal1 = {-1, -1};
    portal2 = {-1, -1};
    keyPos = {-1, -1};
    gatePos = {-1, -1};
    platePos = {-1, -1};
    doorPos = {-1, -1};

    const Level& level = levels[currentLevelIndex];
    for (int y = 0; y < level.getHeight(); ++y) {
        for (int x = 0; x < level.getWidth(); ++x) {
            char tile = level.getTileAt(x, y);
            if (tile == 'O') {
                if (portal1.x == -1) {
                    portal1 = {x, y};
                } else {
                    portal2 = {x, y};
                }
            } else if (tile == 'K') {
                keyPos = {x, y};
            } else if (tile == 'L') {
                gatePos = {x, y};
            } else if (tile == 'S') {
                platePos = {x, y};
            } else if (tile == 'D') {
                doorPos = {x, y};
            }
        }
    }
}

Position Game::computeIceSlide(Position start, Direction direction, int movedBoxIndex) {
    Position delta = {0, 0};
    if (direction == Direction::Up) delta = {0, -1};
    else if (direction == Direction::Down) delta = {0, 1};
    else if (direction == Direction::Left) delta = {-1, 0};
    else if (direction == Direction::Right) delta = {1, 0};

    if (delta.x == 0 && delta.y == 0) {
        return start;
    }

    const Level& level = levels[currentLevelIndex];
    Position current = start;
    for (int i = 0; i < 2; ++i) {
        Position nextPos = {current.x + delta.x, current.y + delta.y};
        if (level.isWall(nextPos.x, nextPos.y, keyCollected, doorOpen)) {
            break;
        }
        bool boxCollision = false;
        for (int b = 0; b < static_cast<int>(boxes.size()); ++b) {
            if (b == movedBoxIndex) continue;
            if (boxes[b].getPosition() == nextPos) {
                boxCollision = true;
                break;
            }
        }
        if (boxCollision) {
            break;
        }
        if (movedBoxIndex >= 0 && player.getPosition() == nextPos) {
            break;
        }
        current = nextPos;
    }
    return current;
}

Position Game::computeTeleport(Position start, int movedBoxIndex) {
    if (portal1.x == -1 || portal2.x == -1) {
        return start;
    }
    Position target = (start == portal1) ? portal2 : portal1;

    if (movedBoxIndex >= 0 && player.getPosition() == target) {
        return start;
    }
    for (int b = 0; b < static_cast<int>(boxes.size()); ++b) {
        if (b == movedBoxIndex) continue;
        if (boxes[b].getPosition() == target) {
            return start;
        }
    }
    return target;
}

void Game::updatePlateTrigger() {
    if (platePos.x == -1) {
        doorOpen = false;
        return;
    }
    if (player.getPosition() == platePos) {
        doorOpen = true;
        return;
    }
    for (const Box& box : boxes) {
        if (box.getPosition() == platePos) {
            doorOpen = true;
            return;
        }
    }
    doorOpen = false;
}

void Game::refreshHint() {
    activeHint = solver.getNextMove(levels[currentLevelIndex], player.getPosition(), boxes, keyCollected, doorOpen);
}

void Game::drawHintArrow(float size, sf::Vector2f offset) {
    const Position position = player.getPosition();
    const float cx = offset.x + position.x * size + size / 2.0f;
    // Pindahkan posisi cy ke atas kepala (di atas tile player)
    const float cy = offset.y + position.y * size - size * 0.15f;
    
    // Perkecil ukuran panah
    const float tip = size * 0.2f;
    const float wing = size * 0.1f;

    sf::ConvexShape arrow(3);
    if (activeHint == Direction::Up) {
        arrow.setPoint(0, {cx, cy - tip});
        arrow.setPoint(1, {cx - wing, cy + wing});
        arrow.setPoint(2, {cx + wing, cy + wing});
    } else if (activeHint == Direction::Down) {
        arrow.setPoint(0, {cx, cy + tip});
        arrow.setPoint(1, {cx - wing, cy - wing});
        arrow.setPoint(2, {cx + wing, cy - wing});
    } else if (activeHint == Direction::Left) {
        arrow.setPoint(0, {cx - tip, cy});
        arrow.setPoint(1, {cx + wing, cy - wing});
        arrow.setPoint(2, {cx + wing, cy + wing});
    } else if (activeHint == Direction::Right) {
        arrow.setPoint(0, {cx + tip, cy});
        arrow.setPoint(1, {cx - wing, cy - wing});
        arrow.setPoint(2, {cx - wing, cy + wing});
    }

    arrow.setFillColor(sf::Color(250, 204, 21, 210));
    arrow.setOutlineThickness(2.0f);
    arrow.setOutlineColor(sf::Color(113, 63, 18));
    window.draw(arrow);
}

bool Game::isComplete() const {
    const Level& level = levels[currentLevelIndex];
    for (const Box& box : boxes) {
        const Position position = box.getPosition();
        if (!level.isGoal(position.x, position.y)) {
            return false;
        }
    }
    return !boxes.empty();
}

void Game::saveBestScore(int elapsed) {
    if (scoreSaved) {
        return;
    }
    const Level& level = levels[currentLevelIndex];
    if (leaderboard.isNewRecord(level.getName(), moveCount, elapsed)) {
        leaderboard.addScore(level.getName(), moveCount, elapsed, "Player");
    }
    scoreSaved = true;
}

float Game::tileSize() const {
    const Level& level = levels[currentLevelIndex];
    // The fence border adds 1 tile on each side, so total tiles = (width+2) x (height+2).
    // Usable horizontal space: window width minus small edge padding.
    // Usable vertical space:   window height minus HUD bottom (92) minus bottom padding.
    const float usableW = static_cast<float>(window.getSize().x) - 18.0f;   // 882 for 900px window
    const float usableH = static_cast<float>(window.getSize().y) - 92.0f - 8.0f; // 620 for 720px window
    return std::floor(std::min(usableW  / static_cast<float>(level.getWidth()  + 2),
                               usableH  / static_cast<float>(level.getHeight() + 2)));
}

sf::Vector2f Game::boardOffset(float size) const {
    const Level& level = levels[currentLevelIndex];
    // Total width / height including the fence ring
    const float totalW = static_cast<float>(level.getWidth()  + 2) * size;
    const float totalH = static_cast<float>(level.getHeight() + 2) * size;
    // Center the fence+board block horizontally; vertically start just below HUD
    const float fenceLeft = (static_cast<float>(window.getSize().x) - totalW) / 2.0f;
    const float fenceTop  = 92.0f + (620.0f - totalH) / 2.0f;
    // boardOffset = top-left of the actual grid (one tile inside the fence)
    return { fenceLeft + size, fenceTop + size };
}

bool Game::runVerificationTests() {
    std::cout << "[TEST] Running gameplay mechanics verification tests..." << std::endl;
    
    // 1. Load the showcase level
    loadLevel(20);
    std::cout << "[TEST] Loaded Level 21: " << levels[currentLevelIndex].getName() << std::endl;
    
    // Assert initial state
    if (!(player.getPosition() == Position{1, 1})) {
        std::cerr << "[TEST FAILED] Expected player initial position (1, 1), got " 
                  << player.getPosition().x << "," << player.getPosition().y << std::endl;
        return false;
    }
    if (keyCollected) {
        std::cerr << "[TEST FAILED] Expected keyCollected to be false initially" << std::endl;
        return false;
    }
    if (doorOpen) {
        std::cerr << "[TEST FAILED] Expected doorOpen to be false initially" << std::endl;
        return false;
    }

    // Verify initial hint from solver
    Direction initialHint = solver.getNextMove(levels[currentLevelIndex], player.getPosition(), boxes, keyCollected, doorOpen);
    if (initialHint != Direction::Down) {
        std::cerr << "[TEST FAILED] Expected initial hint to be Down, got " << static_cast<int>(initialHint) << std::endl;
        return false;
    }
    std::cout << "[TEST PASSED] Initial hint is correctly Down." << std::endl;

    // --- TEST 1: Ice sliding, Teleport, Key, Gate walk-through, and Undo ---
    std::cout << "[TEST] Running Test 1: Ice slide, Teleport, Key collection, Gate walk-through..." << std::endl;
    // Move Right 3 times
    movePlayer(Direction::Right); // to (2, 1)
    movePlayer(Direction::Right); // to (3, 1)
    movePlayer(Direction::Right); // onto (4, 1) (Ice) -> slides to (6, 1)
    if (!(player.getPosition() == Position{6, 1})) {
        std::cerr << "[TEST FAILED] Expected player position after Ice slide to be (6, 1), got " 
                  << player.getPosition().x << "," << player.getPosition().y << std::endl;
        return false;
    }

    // Move to Portal 1
    movePlayer(Direction::Down);  // to (6, 2)
    movePlayer(Direction::Left);  // to (5, 2)
    movePlayer(Direction::Left);  // to (4, 2)
    movePlayer(Direction::Left);  // to (3, 2) (Portal 1) -> teleports to (10, 2)
    if (!(player.getPosition() == Position{10, 2})) {
        std::cerr << "[TEST FAILED] Expected player position after Portal teleport to be (10, 2), got " 
                  << player.getPosition().x << "," << player.getPosition().y << std::endl;
        return false;
    }

    // Collect Key
    movePlayer(Direction::Up);    // to (10, 1) -> collects key
    if (!keyCollected) {
        std::cerr << "[TEST FAILED] Expected keyCollected to be true after visiting (10, 1)" << std::endl;
        return false;
    }

    // Walk through Gate L to (7, 4)
    movePlayer(Direction::Right); // to (11, 1)
    movePlayer(Direction::Down);  // to (11, 2)
    movePlayer(Direction::Down);  // to (11, 3)
    movePlayer(Direction::Left);  // to (10, 3)
    movePlayer(Direction::Left);  // to (9, 3)
    movePlayer(Direction::Left);  // to (8, 3)
    movePlayer(Direction::Down);  // to (8, 4) (Gate L)
    movePlayer(Direction::Left);  // to (7, 4)
    if (!(player.getPosition() == Position{7, 4})) {
        std::cerr << "[TEST FAILED] Expected player position after walking through Gate L to be (7, 4), got " 
                  << player.getPosition().x << "," << player.getPosition().y << std::endl;
        return false;
    }
    std::cout << "[TEST PASSED] Gate L unlocking and walk-through succeeded." << std::endl;

    // Undo all 16 moves to return to start
    std::cout << "[TEST] Undoing Test 1 moves..." << std::endl;
    for (int i = 0; i < 16; ++i) {
        if (!undoStack.empty()) {
            const MoveRecord record = undoStack.back();
            undoStack.pop_back();
            player.setPosition(record.playerBefore);
            if (record.movedBoxIndex >= 0 && record.movedBoxIndex < static_cast<int>(boxes.size())) {
                boxes[record.movedBoxIndex].moveTo(record.boxBefore);
            }
            keyCollected = record.keyCollectedBefore;
            doorOpen = record.doorOpenBefore;
            moveCount = std::max(0, moveCount - 1);
        }
    }
    if (!(player.getPosition() == Position{1, 1}) || keyCollected || doorOpen) {
        std::cerr << "[TEST FAILED] Expected state to be reset after undos, got player: "
                  << player.getPosition().x << "," << player.getPosition().y 
                  << ", keyCollected: " << keyCollected << ", doorOpen: " << doorOpen << std::endl;
        return false;
    }
    std::cout << "[TEST PASSED] Undo system successfully restored initial state." << std::endl;

    // --- TEST 2: Pressure Plate & Door ---
    std::cout << "[TEST] Running Test 2: Pressure plate and Door D opening..." << std::endl;
    // Repeat the 15 moves to get back to (7, 4) and have the key
    movePlayer(Direction::Right); // to (2, 1)
    movePlayer(Direction::Right); // to (3, 1)
    movePlayer(Direction::Right); // onto (4, 1) (Ice) -> slides to (6, 1)
    movePlayer(Direction::Down);  // to (6, 2)
    movePlayer(Direction::Left);  // to (5, 2)
    movePlayer(Direction::Left);  // to (4, 2)
    movePlayer(Direction::Left);  // to (3, 2) (Portal 1) -> teleports to (10, 2)
    movePlayer(Direction::Up);    // to (10, 1) -> collects key
    movePlayer(Direction::Right); // to (11, 1)
    movePlayer(Direction::Down);  // to (11, 2)
    movePlayer(Direction::Down);  // to (11, 3)
    movePlayer(Direction::Left);  // to (10, 3)
    movePlayer(Direction::Left);  // to (9, 3)
    movePlayer(Direction::Left);  // to (8, 3)
    movePlayer(Direction::Down);  // to (8, 4) (Gate L)
    movePlayer(Direction::Left);  // to (7, 4)

    // Position above Box 2 (which is at (5, 3))
    movePlayer(Direction::Up);    // to (7, 3)
    movePlayer(Direction::Left);  // to (6, 3)
    movePlayer(Direction::Up);    // to (6, 2)
    movePlayer(Direction::Left);  // to (5, 2)
    // Push Box 2 down to (5, 6)
    movePlayer(Direction::Down);  // Box 2 to (5, 4)
    movePlayer(Direction::Down);  // Box 2 to (5, 5)
    movePlayer(Direction::Down);  // Box 2 to (5, 6)
    if (!(boxes[1].getPosition() == Position{5, 6})) {
        std::cerr << "[TEST FAILED] Expected Box 2 to be at (5, 6), got "
                  << boxes[1].getPosition().x << "," << boxes[1].getPosition().y << std::endl;
        return false;
    }

    // Position right of Box 1 (which is at (3, 3))
    movePlayer(Direction::Left);  // to (4, 5)
    movePlayer(Direction::Up);    // to (4, 4)
    movePlayer(Direction::Up);    // to (4, 3)
    // Push Box 1 left to (2, 3)
    movePlayer(Direction::Left);  // Box 1 to (2, 3), player to (3, 3)
    if (!(boxes[0].getPosition() == Position{2, 3})) {
        std::cerr << "[TEST FAILED] Expected Box 1 to be at (2, 3), got "
                  << boxes[0].getPosition().x << "," << boxes[0].getPosition().y << std::endl;
        return false;
    }

    // Walk around Box 1 to push it down onto Plate S at (2, 5)
    movePlayer(Direction::Down);  // to (3, 4)
    movePlayer(Direction::Left);  // to (2, 4)
    movePlayer(Direction::Left);  // to (1, 4)
    movePlayer(Direction::Up);    // to (1, 3)
    movePlayer(Direction::Up);    // to (1, 2)
    movePlayer(Direction::Right); // to (2, 2)
    // Push Box 1 down onto Plate S at (2, 5)
    movePlayer(Direction::Down);  // Box 1 to (2, 4)
    movePlayer(Direction::Down);  // Box 1 to (2, 5) (Plate S)
    if (!(boxes[0].getPosition() == Position{2, 5})) {
        std::cerr << "[TEST FAILED] Expected Box 1 to be at (2, 5), got "
                  << boxes[0].getPosition().x << "," << boxes[0].getPosition().y << std::endl;
        return false;
    }
    if (!doorOpen) {
        std::cerr << "[TEST FAILED] Expected doorOpen to be true with Box 1 on plate (2, 5)" << std::endl;
        return false;
    }
    std::cout << "[TEST PASSED] Pressure plate opened Door D successfully." << std::endl;

    // Undo all 35 moves to return to initial state
    std::cout << "[TEST] Undoing Test 2 moves..." << std::endl;
    for (int i = 0; i < 35; ++i) {
        if (!undoStack.empty()) {
            const MoveRecord record = undoStack.back();
            undoStack.pop_back();
            player.setPosition(record.playerBefore);
            if (record.movedBoxIndex >= 0 && record.movedBoxIndex < static_cast<int>(boxes.size())) {
                boxes[record.movedBoxIndex].moveTo(record.boxBefore);
            }
            keyCollected = record.keyCollectedBefore;
            doorOpen = record.doorOpenBefore;
            moveCount = std::max(0, moveCount - 1);
        }
    }
    if (!(player.getPosition() == Position{1, 1}) || keyCollected || doorOpen) {
        std::cerr << "[TEST FAILED] Expected state to be reset after undos, got player: "
                  << player.getPosition().x << "," << player.getPosition().y 
                  << ", keyCollected: " << keyCollected << ", doorOpen: " << doorOpen << std::endl;
        return false;
    }
    std::cout << "[TEST PASSED] Undo system successfully restored initial state again." << std::endl;

    // --- TEST 3: Optimal 40-move Solution Playthrough & Solver Hints ---
    std::cout << "[TEST] Running Test 3: Playing optimal 40-move solution sequence..." << std::endl;
    const std::string moves = "DDDRRURRRRRRURUDURDDDDDRDLLUUURUULUDDDDD";
    for (int i = 0; i < 40; ++i) {
        Direction dir = Direction::None;
        switch (moves[i]) {
            case 'U': dir = Direction::Up; break;
            case 'D': dir = Direction::Down; break;
            case 'L': dir = Direction::Left; break;
            case 'R': dir = Direction::Right; break;
        }

        Direction nextHint = solver.getNextMove(levels[currentLevelIndex], player.getPosition(), boxes, keyCollected, doorOpen);
        char hintChar = 'N';
        if (nextHint == Direction::Up) hintChar = 'U';
        else if (nextHint == Direction::Down) hintChar = 'D';
        else if (nextHint == Direction::Left) hintChar = 'L';
        else if (nextHint == Direction::Right) hintChar = 'R';
        
        std::cout << "[TEST TRACE] Step " << i << ": player=" << player.getPosition().x << "," << player.getPosition().y
                  << " move=" << moves[i] << " hint=" << hintChar << std::endl;

        if (nextHint != dir) {
            std::cerr << "[TEST FAILED] Step " << i << ": Expected hint " << moves[i] << ", got " << hintChar << std::endl;
            return false;
        }

        movePlayer(dir);
    }

    // Verify completion
    if (!isComplete()) {
        std::cerr << "[TEST FAILED] Expected level 21 to be complete, but isComplete() returned false" << std::endl;
        return false;
    }
    std::cout << "[TEST PASSED] Level 21 optimal solution solved and completed successfully!" << std::endl;

    return true;
}

bool Game::isInForbiddenArea(sf::Vector2f pos, float margin) const {
    if (pos.x < margin || pos.x > 900.0f - margin || pos.y < 90.0f + margin || pos.y > 720.0f - margin) {
        return true;
    }
    
    const Level& level = levels[currentLevelIndex];
    const float size = tileSize();
    const sf::Vector2f offset = boardOffset(size);
    
    float boardLeft = offset.x - margin;
    float boardRight = offset.x + level.getWidth() * size + margin;
    float boardTop = offset.y - margin;
    float boardBottom = offset.y + level.getHeight() * size + margin;
    
    if (pos.x >= boardLeft && pos.x <= boardRight && pos.y >= boardTop && pos.y <= boardBottom) {
        return true;
    }
    
    return false;
}

void Game::initScenery() {
    backgroundAnimals.clear();
    sceneryDecorations.clear();

    // Spawn 15-25 decorations (flowers, mushrooms, stumps, rocks)
    std::vector<sf::IntRect> decoRects = {
        sf::IntRect(16, 0, 16, 16),   // Flowers 1
        sf::IntRect(16, 16, 16, 16),  // Flowers 2
        sf::IntRect(48, 0, 16, 16),   // Mushrooms
        sf::IntRect(0, 16, 16, 16),   // Stump
        sf::IntRect(0, 48, 16, 16),   // Log
        sf::IntRect(112, 32, 16, 16), // Rock 1
        sf::IntRect(80, 48, 16, 16)   // Rock 2
    };

    int numDecos = 15 + rand() % 11;
    for (int i = 0; i < numDecos; ++i) {
        sf::Vector2f pos;
        int attempts = 0;
        do {
            pos.x = static_cast<float>(30 + rand() % 840);
            pos.y = static_cast<float>(100 + rand() % 580);
            attempts++;
        } while (isInForbiddenArea(pos, 20.0f) && attempts < 100);
        
        if (attempts < 100) {
            SceneryDeco deco;
            deco.position = pos;
            deco.rect = decoRects[rand() % decoRects.size()];
            deco.scale = 2.0f;
            sceneryDecorations.push_back(deco);
        }
    }
}
