#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>  // For file handling

sf::Text renderText(const std::string& message, float x, float y, const sf::Color& color, 
                    const sf::Font& font, unsigned int size, sf::RenderWindow& window) {
    sf::Text text;
    text.setFont(font);
    text.setString(message);
    text.setCharacterSize(size);
    text.setFillColor(color);

    // Center the text origin
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.width / 2, textRect.height / 2);
    
    // Set the position
    text.setPosition(x, y);

    // Draw the text onto the window
    window.draw(text);

    return text; // Optional: return the text object if needed
}
// Declare GameState enum before use
enum GameState {
    SPLASH_SCREEN,
    GAME_SCREEN,
    GAME_OVER_SCREEN,
    GAME_WON_SCREEN,
    SETTINGS_SCREEN,
    PROFILE_SCREEN,
    LEVELS_SCREEN
};

// Declare changeState function before using it
void changeState(GameState newState);

// Mapping of screen names corresponding to each GameState
const char* screenNames[] = {
    "Splash Screen",
    "Game Screen",
    "Game Over Screen",
    "Game Won Screen",
    "Settings Screen",
    "Profile Screen",
    "Levels Screen"
};

// Profile data (in a simple structure for this example)
struct PlayerProfile {
    int highestLevelUnlocked = 1; // Default to level 1
    int score = 0;
};

// Player's profile data (it will be read from a file or stored in memory)
PlayerProfile playerProfile;

// Game States
class State {
public:
    virtual void handleEvents(sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual ~State() {}
};

// Tetris Game Implementation (simplified for now)
class Tetris {
private:
    static const int WIDTH = 10;
    static const int HEIGHT = 20;
    int grid[WIDTH][HEIGHT] = {0};

    sf::RectangleShape blockShape;

public:
    Tetris() {
        blockShape.setSize(sf::Vector2f(30, 30)); // Size of each block
        blockShape.setFillColor(sf::Color::Cyan); // Default color
    }

    void render(sf::RenderWindow& window) {
        // Render the grid and active blocks
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                if (grid[x][y] != 0) {
                    blockShape.setPosition(x * 30, y * 30);
                    window.draw(blockShape);
                }
            }
        }
    }

    void addBlock(int x, int y, int color) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            grid[x][y] = color; // Mark the grid with the block color
        }
    }

    void clearFullLines() {
        for (int y = HEIGHT - 1; y >= 0; y--) {
            bool lineFull = true;
            for (int x = 0; x < WIDTH; x++) {
                if (grid[x][y] == 0) {
                    lineFull = false;
                    break;
                }
            }
            if (lineFull) {
                for (int x = 0; x < WIDTH; x++) {
                    grid[x][y] = 0;
                }
                // Shift everything down
                for (int yy = y - 1; yy >= 0; yy--) {
                    for (int xx = 0; xx < WIDTH; xx++) {
                        grid[xx][yy + 1] = grid[xx][yy];
                    }
                }
            }
        }
    }

    void rotateBlock() {}
    void moveBlockLeft() {}
    void moveBlockRight() {}
    void moveBlockDown() {}
};

// Splash Screen State
////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


class SplashScreenState : public State {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Font fontBindhu; 
    sf::Font fontTimes;  
    sf::Clock clock; 
    sf::Time startTime = sf::Time::Zero;

public:
    SplashScreenState() {
        // Load the background image
        if (!texture.loadFromFile("assets/cloud.png")) {
            std::cerr << "Unable to load image!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setScale(0.5f, 0.5f); 

        // Load the fonts
        if (!fontBindhu.loadFromFile("assets/Bindhu.ttf")) {
            std::cerr << "Unable to load font 'Bindhu.ttf'!" << std::endl;
        }
        if (!fontTimes.loadFromFile("assets/times.ttf")) {
            std::cerr << "Unable to load font 'times.ttf'!" << std::endl;
        }
    }

    void handleEvents(sf::Event& event) override {
        sf::Time currentTime = clock.getElapsedTime();
        if (currentTime.asMilliseconds() - startTime.asMilliseconds() > 9000) { 
            changeState(GAME_SCREEN);
        }
    }

    void update() override {
        // No effects needed anymore, just ensure the splash screen duration is handled.
    }

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 192, 203)); // Set a background color

        // Draw the background image
        window.draw(sprite);

        // Render texts using the global renderText function
        renderText("Fairyland Tetris", 400, 370, sf::Color(255, 105, 125), fontBindhu, 100, window);
        
        // Use `fontTimes` for name, roll number, and class
        renderText("Ayesha Siddiqa", 400, 500, sf::Color::White, fontTimes, 60, window);
        renderText("241419", 400, 550, sf::Color::White, fontTimes, 40, window);
        renderText("AU-BS-AI-24(B)", 400, 620, sf::Color::White, fontTimes, 40, window);

        // Display the updated window
        window.display();
    }
};



//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Game Won Screen
class GameWonScreenState : public State {
public:
    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                playerProfile.highestLevelUnlocked++; // Unlock the next level
                changeState(LEVELS_SCREEN);  // Call changeState here
            }
        }
    }

    void update() override {}

    void render(sf::RenderWindow& window) override {
       window.clear(sf::Color(255, 105, 180)); 
        // Display a winning message
        sf::Font font;
        if (!font.loadFromFile("Arial.ttf")) {
            std::cerr << "Unable to load font!" << std::endl;
        }
        sf::Text text("You Won! Press Space to Continue", font, 30);
        text.setFillColor(sf::Color(255, 105, 180));
        text.setPosition(50, 250);
        window.draw(text);
        window.display();
    }
};
//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
////////////////////////////////////
// Game Screen
class GameScreenState : public State {
private:
    Tetris tetrisGame;

public:
    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                tetrisGame.moveBlockLeft();
            }
            if (event.key.code == sf::Keyboard::Right) {
                tetrisGame.moveBlockRight();
            }
            if (event.key.code == sf::Keyboard::Down) {
                tetrisGame.moveBlockDown();
            }
            if (event.key.code == sf::Keyboard::Up) {
                tetrisGame.rotateBlock();
            }
        }
    }

    void update() override {
        tetrisGame.clearFullLines();
    }

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 192, 203));
        tetrisGame.render(window);
        window.display();
    }
};
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////
/////////////////////////////////
///////////////////////////////////////////////////////
// Levels Screen
class LevelsScreenState : public State {
public:
    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                changeState(GAME_SCREEN);  // Call changeState here
            }
        }
    }

    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color::Blue);
        // Render the level selection screen
        sf::Font font;
        if (!font.loadFromFile("Arial.ttf")) {
            std::cerr << "Unable to load font!" << std::endl;
        }

        sf::Text text("Select Level", font, 40);
        text.setFillColor(sf::Color(255, 105, 180));
        text.setPosition(100, 50);
        window.draw(text);

        // Render levels, locked and unlocked
        for (int i = 1; i <= 5; i++) {
            sf::Text levelText;
            levelText.setFont(font);
            levelText.setCharacterSize(30);
            if (i <= playerProfile.highestLevelUnlocked) {
                levelText.setFillColor(sf::Color::Green);  // Unlocked level
                levelText.setString("Level " + std::to_string(i));
            } else {
                levelText.setFillColor(sf::Color::Red);    // Locked level
                levelText.setString("Level " + std::to_string(i) + " (Locked)");
            }
            levelText.setPosition(100, 100 + i * 40);
            window.draw(levelText);
        }

        window.display();
    }
};
///////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////
//////////////////////////////////////////////////////
// Other states like Profile, Settings can be added similarly

void changeState(GameState newState);

std::unique_ptr<State> currentStateInstance;
GameState currentState = SPLASH_SCREEN;

void changeState(GameState newState) {
    currentState = newState;
    switch (currentState) {
    case SPLASH_SCREEN:
        currentStateInstance = std::make_unique<SplashScreenState>();
        break;
        case LOGIN_SCREEN:
        currentStateInstance = std::make_unique<LoginScreenState>();
        break;
        case SIGNUP_SCREEN:
        currentStateInstance = std::make_unique<SignupScreenState>();
        break;

    case GAME_SCREEN:
        currentStateInstance = std::make_unique<GameScreenState>();
        break;
    case GAME_WON_SCREEN:
        currentStateInstance = std::make_unique<GameWonScreenState>();
        break;
         case GAME_LOST_SCREEN:
        currentStateInstance = std::make_unique<GameLostScreenState>();
        break;
    case LEVELS_SCREEN:
        currentStateInstance = std::make_unique<LevelsScreenState>();
        break;
         case PROFILE_SCREEN:
        currentStateInstance = std::make_unique<ProfileScreenState>();
        break;
         case SETTINGS_SCREEN:
        currentStateInstance = std::make_unique<SettingsScreenState>();
        break;
    default:
        std::cerr << "Unknown State!" << std::endl;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800,800), "Tetris Game");

    // Initial state
    changeState(SPLASH_SCREEN);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            currentStateInstance->handleEvents(event);
        }

        currentStateInstance->update();
        currentStateInstance->render(window);
    }

    return 0;
}
