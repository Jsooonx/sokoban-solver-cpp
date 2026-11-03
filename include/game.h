#pragma once

#include "box.h"
#include "leaderboard.h"
#include "level.h"
#include "player.h"
#include "solver.h"
#include "ui.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>

enum class GameState {
    StartScreen,
    MainMenu,
    Playing,
    Paused,
    LevelComplete,
    GameComplete,
    Leaderboard,
    About
};

enum class AnimalType {
    Cow,
    Chicken
};

enum class AnimalState {
    Idle,
    Walking
};

struct WanderingAnimal {
    AnimalType type;
    sf::Vector2f position;
    sf::Vector2f velocity;
    AnimalState state;
    float stateTimer = 0.0f;
    float frameIndex = 0.0f;
    bool facingLeft = false;
};

struct SceneryDeco {
    sf::Vector2f position;
    sf::IntRect rect;
    float scale = 2.0f;
};

class Game {
public:
    Game();
    void run();
    bool runVerificationTests();

private:
    sf::RenderWindow window;
    std::vector<Level> levels;
    int currentLevelIndex = 0;
    GameState state = GameState::StartScreen;
    Player player;
    std::vector<Box> boxes;
    Leaderboard leaderboard;
    Solver solver;
    UI ui;
    sf::Music backgroundMusic;
    sf::Clock levelClock;
    std::deque<MoveRecord> undoStack;
    int moveCount = 0;
    int hintsUsed = 0;
    int completionTime = 0;
    Direction activeHint = Direction::None;
    bool hintVisible = true;
    bool soundOn = true;
    bool scoreSaved = false;

    void handleInput();
    void update();
    void render();
    void loadLevel(int index);
    void restartLevel();
    void setSoundEnabled(bool enabled);
    void movePlayer(Direction direction);
    void refreshHint();
    void drawHintArrow(float size, sf::Vector2f offset);
    bool isComplete() const;
    void saveBestScore(int elapsed);
    float tileSize() const;
    sf::Vector2f boardOffset(float size) const;

    // Gameplay mechanics expansion states
    Position portal1 = {-1, -1};
    Position portal2 = {-1, -1};
    Position keyPos = {-1, -1};
    Position gatePos = {-1, -1};
    Position platePos = {-1, -1};
    Position doorPos = {-1, -1};
    bool keyCollected = false;
    bool doorOpen = false;

    void cacheLevelElements();
    Position computeIceSlide(Position start, Direction direction, int movedBoxIndex);
    Position computeTeleport(Position start, int movedBoxIndex);
    void updatePlateTrigger();

    // Scenery & Animal textures & states
    sf::Texture cowTexture;
    sf::Texture chickenTexture;
    sf::Texture grassBgTexture;
    sf::Texture fenceTexture;
    sf::Texture decoTexture;

    bool cowTextureLoaded = false;
    bool chickenTextureLoaded = false;
    bool grassBgTextureLoaded = false;
    bool fenceTextureLoaded = false;
    bool decoTextureLoaded = false;

    std::vector<WanderingAnimal> backgroundAnimals;
    std::vector<SceneryDeco> sceneryDecorations;
    sf::Clock updateClock;

    bool isInForbiddenArea(sf::Vector2f pos, float margin = 20.0f) const;
    void initScenery();
};

