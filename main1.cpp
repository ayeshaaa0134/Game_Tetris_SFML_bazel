#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <cmath> 
#include <array>
#include <cstdlib>
#include <ctime>
#include <functional>


// Declare GameState enum
enum GameState {
    //29 documentatation gdd + finalization of both tasks 
    SPLASH_SCREEN,
    LOGIN_SCREEN,
    SIGNUP_SCREEN,//27+ict documentation
    GAME_RUNNING_SCREEN,//major game 28+ ict remaining code merge 
    GAME_OVER_SCREEN,//27
    GAME_WON_SCREEN,//27
    GAME_LOST_SCREEN,//27
    SETTINGS_SCREEN,//27
    PROFILE_SCREEN,//27
    LEVELS_SCREEN//27
};

void changeState(GameState newState);
std::string getStateTitle(GameState state) {
    switch (state) {
        case SPLASH_SCREEN: return "Tetris - Splash Screen";
        case LOGIN_SCREEN: return "Tetris - Login Screen";
        case SIGNUP_SCREEN: return "Tetris - Signup Screen";
        case GAME_RUNNING_SCREEN: return "Tetris - Game Screen";
        case GAME_WON_SCREEN: return "Tetris - Game Won";
        case GAME_LOST_SCREEN: return "Tetris - Game Lost";
        case LEVELS_SCREEN: return "Tetris - Levels Screen";
        case PROFILE_SCREEN: return "Tetris - Profile Screen";
        case SETTINGS_SCREEN: return "Tetris - Settings Screen";
        default: return "Tetris - Unknown State";
    }
}
sf::Font fontBindhu;
sf::Font fontTimes;
const char* screenNames[] = {
    "Splash Screen",
    "Login Screen",
    "Sign-Up Screen",
    "Game Running Screen",
    "Game Over Screen",
    "Game Won Screen",
    "Settings Screen",
    "Profile Screen",
    "Levels Screen"
};

// User profile
struct PlayerProfile {
    std::string username;
    std::string password;
    int highestLevelUnlocked = 1; // Default to level 1
    int score = 0;
};

PlayerProfile playerProfile;
std::unordered_map<std::string, std::string> userDatabase;

// Base State class
class State {
public:
    virtual void handleEvents(sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual ~State() {}
};

// Global variables
std::unique_ptr<State> currentStateInstance;
GameState currentState = SPLASH_SCREEN;


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



    void renderButton(const std::string& text, float x, float y, sf::Color color, sf::Font& font, unsigned int size, sf::RenderWindow& window) {
        sf::RectangleShape button(sf::Vector2f(150, 50));
        button.setFillColor(color);
        button.setPosition(x - 75, y - 25);  // Center the button
        window.draw(button);

        renderText(text, x, y, sf::Color::White, font, size, window);
    }
 void drawRoundedRectangle(float x, float y, float width, float height, float radius, 
                          sf::Color fillColor, sf::Color outlineColor, sf::RenderWindow& window) {
    const int pointCount = 30; // Number of points for smooth corners
    sf::VertexArray vertices(sf::TriangleFan);

    // Add points for the rounded rectangle
    float angleStep = 90.f / pointCount;

    // Top-left corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 180.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                y + radius + radius * std::sin(rad)), fillColor));
    }

    // Top-right corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 270.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                y + radius + radius * std::sin(rad)), fillColor));
    }

    // Bottom-right corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 0.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                y + height - radius + radius * std::sin(rad)), fillColor));
    }

    // Bottom-left corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 90.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                y + height - radius + radius * std::sin(rad)), fillColor));
    }

    // Draw the filled rounded rectangle
    window.draw(vertices);

    // Draw the outline separately
    if (outlineColor != sf::Color::Transparent) {
        sf::VertexArray outline(sf::LineStrip);

        // Add outline points for the rounded rectangle
        for (int i = 0; i <= pointCount; ++i) {
            float angle = 180.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                   y + radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 270.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                   y + radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 0.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                   y + height - radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 90.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                   y + height - radius + radius * std::sin(rad)), outlineColor));
        }

        outline.append(outline[0]); // Close the loop
        window.draw(outline);
    }
}


const int WINNING_LINES = 10;

class Tetris {
private:
    std::vector<std::vector<int>> board;
    sf::RectangleShape blockShape;
    sf::Vector2i currentBlockPosition;
    sf::Texture blockTexture;
    sf::Vector2i currentBlockSize;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::RectangleShape bottomBar;
    sf::Clock clock;
    bool isBlockActive;
    int linesCleared;
    int score;

    sf::Texture scoreBarTexture;
    sf::Sprite scoreBarSprite;
    sf::Font fontBindhu;
    sf::Text scoreText;

    int rotationState;

public:
    Tetris()
        : Tetris(10, 20) { // Default constructor delegates to parameterized constructor
    }

    Tetris(int boardWidth, int boardHeight)
        : board(boardHeight, std::vector<int>(boardWidth, 0)),
          isBlockActive(false),
          linesCleared(0),
          score(0),
          rotationState(0) {
        blockShape.setSize(sf::Vector2f(400,400)); // Increased block size to 50x50

        if (!blockTexture.loadFromFile("assets/star1.png")) {
            std::cerr << "Error loading block texture" << std::endl;
        }
        blockShape.setTexture(&blockTexture);

        if (!backgroundTexture.loadFromFile("assets/cloud.png")) {
            std::cerr << "Error loading background texture" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        bottomBar.setFillColor(sf::Color::White);

        if (!scoreBarTexture.loadFromFile("assets/moon.png")) {
            std::cerr << "Error loading score bar texture" << std::endl;
        }
        scoreBarSprite.setTexture(scoreBarTexture);
        scoreBarSprite.setPosition(1500, 100); 

       
        scoreText.setFont(fontBindhu);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(1500, 150); // Position score text below the score bar

        srand(time(0));
        spawnNewBlock();
    }

    void resizeGameElements(sf::RenderWindow& window) {
        int boardWidth = static_cast<int>(window.getSize().x / blockShape.getSize().x);
        int boardHeight = static_cast<int>(window.getSize().y / blockShape.getSize().y - 1);
        board.resize(boardHeight, std::vector<int>(boardWidth, 0));

       sf::Vector2f backgroundScale(
    static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
    static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
);
backgroundSprite.setScale(backgroundScale);
sf::RectangleShape bottomBar;
bottomBar.setSize(sf::Vector2f(800, 30)); 
bottomBar.setPosition(100, 500);  

    }

   void updateGameLogic() {
    if (!isBlockActive) return;

    if (clock.getElapsedTime().asMilliseconds() >= 3000) {
        currentBlockPosition.y++;

        // After moving the block down, we directly place it and spawn a new block
        placeBlock();
        clearFullLines();
        spawnNewBlock();

        // Restart the clock after placing the block
        clock.restart();
    }
}


    void moveBlockLeft() {
        currentBlockPosition.x--;
        if (checkCollision()) {
            currentBlockPosition.x++;
        }
    }

    void moveBlockRight() {
        currentBlockPosition.x++;
        if (checkCollision()) {
            currentBlockPosition.x--;
        }
    }

    void moveBlockDown() {
        currentBlockPosition.y++;
        if (checkCollision() || touchesBottomBar()) {
            currentBlockPosition.y--;
            placeBlock();
            clearFullLines();
            spawnNewBlock();
        }
    }

    void rotateBlock() {
        rotationState = (rotationState + 1) % 4;
        if (checkCollision()) {
            rotationState = (rotationState - 1 + 4) % 4;
        }
    }

    bool touchesBottomBar() {
        return currentBlockPosition.y + static_cast<int>(currentBlockSize.y / blockShape.getSize().x) >= static_cast<int>(board.size());
    }

    void placeBlock() {
        for (int x = currentBlockPosition.x; x < currentBlockPosition.x + static_cast<int>(currentBlockSize.x / blockShape.getSize().x); ++x) {
            for (int y = currentBlockPosition.y; y < currentBlockPosition.y + static_cast<int>(currentBlockSize.y / blockShape.getSize().y); ++y) {
                if (x >= 0 && x < static_cast<int>(board[0].size()) && y >= 0 && y < static_cast<int>(board.size())) {
                    board[y][x] = 1;
                }
            }
        }
    }

    void clearFullLines() {
        for (int y = 0; y < static_cast<int>(board.size()); ++y) {
            bool lineFull = true;
            for (int x = 0; x < static_cast<int>(board[0].size()); ++x) {
                if (board[y][x] == 0) {
                    lineFull = false;
                    break;
                }
            }

            if (lineFull) {
                for (int x = 0; x < static_cast<int>(board[0].size()); ++x) {
                    board[y][x] = 0;
                }

                for (int i = y; i > 0; --i) {
                    for (int x = 0; x < static_cast<int>(board[0].size()); ++x) {
                        board[i][x] = board[i - 1][x];
                    }
                }

                linesCleared++;
                score += 100;
                scoreText.setString("Score: " + std::to_string(score));
            }
        }
    }

    void render(sf::RenderWindow& window) {
        window.draw(backgroundSprite);
        window.draw(bottomBar);
        window.draw(scoreBarSprite);
        window.draw(scoreText);

        for (int y = 0; y < static_cast<int>(board.size()); ++y) {
            for (int x = 0; x < static_cast<int>(board[0].size()); ++x) {
                if (board[y][x] != 0) {
                    blockShape.setPosition(static_cast<float>(x * blockShape.getSize().x),
                                           static_cast<float>(y * blockShape.getSize().y));
                    window.draw(blockShape);
                }
            }
        }

        blockShape.setPosition(static_cast<float>(currentBlockPosition.x * blockShape.getSize().x),
                               static_cast<float>(currentBlockPosition.y * blockShape.getSize().y));
        window.draw(blockShape);
    }

    bool checkCollision() {
        if (currentBlockPosition.x < 0 || currentBlockPosition.x >= static_cast<int>(board[0].size()) ||
            currentBlockPosition.y >= static_cast<int>(board.size())) {
            return true;
        }

        for (int x = currentBlockPosition.x; x < currentBlockPosition.x + static_cast<int>(currentBlockSize.x / blockShape.getSize().x); ++x) {
            for (int y = currentBlockPosition.y; y < currentBlockPosition.y + static_cast<int>(currentBlockSize.y / blockShape.getSize().y); ++y) {
                if (x >= 0 && x < static_cast<int>(board[0].size()) && y >= 0 && y < static_cast<int>(board.size())) {
                    if (board[y][x] != 0) {
                        return true;
                    }
                }
            }
        }

        return false;
    }
void spawnNewBlock() {
    isBlockActive = true;
    currentBlockSize = sf::Vector2i(400,400);
   int randX = static_cast<int>(rand() % board[0].size());
currentBlockPosition = { randX, 0 };



    if (checkCollision()) {
        isBlockActive = false; // Game over scenario
    }
}

bool isGameOver() {
    for (int x = 0; x < static_cast<int>(board[0].size()); ++x) {
        if (board[0][x] != 0) {
            return true;
        }
    }
    return false;
}

bool isGameWon() {
    return linesCleared >= WINNING_LINES;
}

};

// Function to randomly choose a sentence from the list
std::string getRandomSentence() {
    std::vector<std::string> sentences = {
       "Believe in yourself, always.",
       "Stay strong.",
       "You are stronger than you know.",
       "Never give up.",
       "Your potential is endless.",
       "You are enough.",
"Best Of luck!",
"Go get it!"

    };
    
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(0)));
    
    // Select a random index
    int randomIndex = std::rand() % sentences.size();
    
    // Return the randomly chosen sentence
    return sentences[randomIndex];
}


// Splash Screen State
class SplashScreenState : public State {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Texture butterflyTexture;  // Texture for the butterfly
    sf::Sprite butterflySpriteLeft;  // Butterfly sprite on the left side
    sf::Sprite butterflySpriteRight; // Butterfly sprite on the right side
    sf::Font fontBindhu; 
    sf::Font fontTimes;  
    sf::Clock clock; 
    sf::Time startTime = sf::Time::Zero;

public:
    SplashScreenState() : startTime(clock.getElapsedTime()) {
        // Load the background image
        if (!texture.loadFromFile("assets/cloud.png")) {
            std::cerr << "Unable to load image!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setScale(0.7f, 0.7f); 

        // Load the butterfly image
        if (!butterflyTexture.loadFromFile("assets/butterfly.png")) {
            std::cerr << "Unable to load butterfly image!" << std::endl;
        }
        // Set up the butterfly sprites for the left and right sides
        butterflySpriteLeft.setTexture(butterflyTexture);
        butterflySpriteLeft.setScale(0.3f, 0.3f); // Adjust the scale of the butterfly
        butterflySpriteLeft.setPosition(100, 100); // Position the butterfly on the left side

        butterflySpriteRight.setTexture(butterflyTexture);
        butterflySpriteRight.setScale(0.3f, 0.3f); // Adjust the scale of the butterfly
        butterflySpriteRight.setPosition(1200, 100); // Position the butterfly on the right side (adjust as needed)

        // Load the fonts
        if (!fontBindhu.loadFromFile("assets/Bindhu.ttf")) {
            std::cerr << "Unable to load font 'Bindhu.ttf'!" << std::endl;
        }
        if (!fontTimes.loadFromFile("assets/times.ttf")) {
            std::cerr << "Unable to load font 'times.ttf'!" << std::endl;
        }
    }

    void handleEvents(sf::Event& event) override {
        // Handle events if necessary
    }

    void update() override {
        sf::Time currentTime = clock.getElapsedTime();
        if (currentTime.asMilliseconds() - startTime.asMilliseconds() > 9000) { 
            changeState(LOGIN_SCREEN);
        }
    }

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 192, 203)); // Set a background color

        // Draw the background image
        window.draw(sprite);

        // Draw the butterfly sprites (left and right)
        window.draw(butterflySpriteLeft);
        window.draw(butterflySpriteRight);

        // Render texts using the global renderText function
        renderText("Fairyland Tetris", 943, 420, sf::Color(255, 105, 125), fontBindhu, 100, window);
        renderText("Ayesha Siddiqa", 943, 500, sf::Color(255, 105, 125), fontTimes, 60, window);
        renderText("AU-BS-AI-24(B)", 943, 570, sf::Color(255, 105, 125), fontTimes, 50, window);
        renderText("241419", 943, 630, sf::Color(255, 105, 125), fontTimes, 50, window);
       
 for (int i = 0; i < 1; ++i) { // You can adjust the loop to render the sentence multiple times if needed
            // Get a random sentence
            std::string sentence = getRandomSentence();
            
            // Render the random sentence
            renderText(sentence, 950, 950, sf::Color::White, fontBindhu, 80, window);
        }


        // Display the updated window
        window.display();
    }
};


/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////
// Login Screen State

class LoginScreenState : public State {
private:
    std::string usernameInput;
    std::string passwordInput;
    bool usernameEntered = false;
    bool showPassword = false;
    bool showError = false;
    sf::Texture bowTexture;
    sf::Texture birdTexture;
    sf::Sprite bowSprite;
    sf::Sprite birdSprite;

public:
    LoginScreenState() {
        // Load images for the bow and bird
        if (!bowTexture.loadFromFile("assets/bow.png")) {
            // Handle error
        }
        bowSprite.setTexture(bowTexture);

        if (!birdTexture.loadFromFile("assets/mother.png")) {
            // Handle error
        }
        birdSprite.setTexture(birdTexture);
    }

   void handleEvents(sf::Event& event) override {
    // Handle text input for username and password
    if (event.type == sf::Event::TextEntered) {
        if (!usernameEntered) {
            // Handle input for the username
            if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                // Enter key moves to password entry
                usernameEntered = true;
            } else if (event.text.unicode == 8 && !usernameInput.empty()) { // Backspace
                usernameInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Ignore non-printable and non-ASCII
                usernameInput += static_cast<char>(event.text.unicode);
            }
        } else {
            // Handle input for the password
            if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                // Enter key checks the login credentials
                if (usernameInput == "user" && passwordInput == "pass") {
                    changeState(LEVELS_SCREEN); // Valid credentials
                } else {
                    showError = true; // Invalid credentials
                }
            } else if (event.text.unicode == 8 && !passwordInput.empty()) { // Backspace
                passwordInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Ignore non-printable and non-ASCII
                passwordInput += static_cast<char>(event.text.unicode);
            }
        }
    }
    // Handle mouse clicks for buttons and checkbox
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f clickPos(event.mouseButton.x, event.mouseButton.y);

            // Checkbox: Toggle password visibility
            sf::FloatRect checkboxBounds(400, 340, 23, 23); // Adjust position and size
            if (checkboxBounds.contains(clickPos)) {
                showPassword = !showPassword;
            }

            // "Sign Up" button: Navigate to Sign-Up screen
            sf::FloatRect signUpButtonBounds(950, 760, 200, 58); // Adjust position and size
            if (signUpButtonBounds.contains(clickPos)) {
                changeState(SIGNUP_SCREEN);
            }

            // "Login" button: Validate login credentials
            sf::FloatRect loginButtonBounds(750, 760, 200, 58); // Adjust position and size
            if (loginButtonBounds.contains(clickPos)) {
                if (usernameInput == "user" && passwordInput == "pass") {
                    changeState(GAME_RUNNING_SCREEN); // Valid login
                } else {
                    showError = true; // Invalid login
                }
            }
        }
    }
}

    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 192, 203));

        // Title
        renderText("Fairyland Tetris", 810, 210, sf::Color::White, fontBindhu, 66, window);

        // Login box
        sf::RectangleShape loginBox(sf::Vector2f(740, 740));
        loginBox.setFillColor(sf::Color(255, 192, 203));
        loginBox.setOutlineColor(sf::Color::White);
        loginBox.setOutlineThickness(3);
        loginBox.setPosition(600, 290);
        window.draw(loginBox);

        // Bow image at the top-left corner of the login box
        bowSprite.setScale(0.3f, 0.3f);
        bowSprite.setPosition(456, 132);
        window.draw(bowSprite);

        // Bird image at the right-most side of the login box
        birdSprite.setScale(0.5f, 0.5f);
        birdSprite.setPosition(2150 - birdSprite.getGlobalBounds().width, 370);
        window.draw(birdSprite);

        // Instruction text
        renderText("Login to enter the Fairyland", 675, 400, sf::Color::White, fontBindhu, 56, window);

        // Username label and box
        renderText("Username", 640, 510, sf::Color::White, fontTimes, 50, window);
        drawRoundedRectangle(850, 510, 350, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText(usernameInput.empty() ? "Username" : usernameInput, 920, 518, sf::Color(192, 192, 192), fontTimes, 33, window);

        // Password label and box
        renderText("Password", 640, 580, sf::Color::White, fontTimes, 50, window);
        drawRoundedRectangle(850, 580, 350, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText(passwordInput.empty() ? "Password" : (showPassword ? passwordInput : std::string(passwordInput.size(), '*')), 920, 588, sf::Color(192, 192, 192), fontTimes, 33, window);

        // Show Password checkbox
        renderText("Show Password", 880, 650, sf::Color::White, fontTimes, 37, window);
        sf::RectangleShape checkbox(sf::Vector2f(33, 33));
        checkbox.setFillColor(showPassword ? sf::Color(0, 255, 0) : sf::Color::White);
        checkbox.setOutlineColor(sf::Color(255, 192, 203));
        checkbox.setOutlineThickness(2);
        checkbox.setPosition(830, 650);
        window.draw(checkbox);

        // Error message
        if (showError) {
            renderText("Invalid username or password!", 710, 690, sf::Color::Red, fontTimes, 33, window);
        }

        // Buttons
        drawRoundedRectangle(730, 760, 200, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText("Login", 790, 755, sf::Color(192, 192, 192), fontTimes, 45, window);

        drawRoundedRectangle(950, 760, 200, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText("Sign Up", 995, 755, sf::Color(192, 192, 192), fontTimes, 45, window);

        renderText("Forgot Password ?", 790, 850, sf::Color::White, fontTimes, 45, window);

        window.display();
    }

   void drawRoundedRectangle(float x, float y, float width, float height, float radius, 
                          sf::Color fillColor, sf::Color outlineColor, sf::RenderWindow& window) {
    const int pointCount = 30; // Number of points for smooth corners
    sf::VertexArray vertices(sf::TriangleFan);

    // Add points for the rounded rectangle
    float angleStep = 90.f / pointCount;

    // Top-left corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 180.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                y + radius + radius * std::sin(rad)), fillColor));
    }

    // Top-right corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 270.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                y + radius + radius * std::sin(rad)), fillColor));
    }

    // Bottom-right corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 0.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                y + height - radius + radius * std::sin(rad)), fillColor));
    }

    // Bottom-left corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 90.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                y + height - radius + radius * std::sin(rad)), fillColor));
    }

    // Draw the filled rounded rectangle
    window.draw(vertices);

    // Draw the outline separately
    if (outlineColor != sf::Color::Transparent) {
        sf::VertexArray outline(sf::LineStrip);

        // Add outline points for the rounded rectangle
        for (int i = 0; i <= pointCount; ++i) {
            float angle = 180.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                   y + radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 270.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                   y + radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 0.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                   y + height - radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 90.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                   y + height - radius + radius * std::sin(rad)), outlineColor));
        }

        outline.append(outline[0]); // Close the loop
        window.draw(outline);
    }
}
    void renderText(const std::string& text, float x, float y, const sf::Color& color, const sf::Font& font, unsigned int size, sf::RenderWindow& window) {
        sf::Text sfText;
        sfText.setFont(font);
        sfText.setString(text);
        sfText.setCharacterSize(size);
        sfText.setFillColor(color);
        sfText.setPosition(x, y);
        window.draw(sfText);
    }
};

/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////////////////////////////////
///////////////////////////////////////////////////////////////
//////////////////////////

class SignUpScreenState : public State {
private:
    std::string usernameInput;
    std::string emailInput;
    std::string PasswordInput;
    std::string ConfirmPasswordInput;
    bool usernameEntered = false;
    bool emailEntered = false;
    bool PasswordEntered = false;
    bool ConfirmPasswordEntered = false;
  
    bool showError = false;
    bool passwordMismatch = false;
    sf::Texture starTexture;
    std::vector<sf::Sprite> starSprites;

public:
    SignUpScreenState() {
        // Load image for the star
        if (!starTexture.loadFromFile("assets/star.png")) {
            // Handle error
        }

      // Create multiple stars with different sizes and positions
for (int i = 0; i < 20; ++i) { // Increase the number of stars
    sf::Sprite star;
    star.setTexture(starTexture);

    // Smaller and random sizes
    float scale = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.05f));
    star.setScale(scale, scale);

    // Random positions to create a flowing effect
    float xPos = 100 + std::rand() % 400; // Flowing horizontally
    float yPos = 150 + std::rand() % 300; // Random vertical spread
    star.setPosition(xPos, yPos);

    starSprites.push_back(star);
}

for (int i = 0; i < 20; ++i) { // Increase the number of stars for lower right
    sf::Sprite star;
    star.setTexture(starTexture);

    // Smaller and random sizes
    float scale = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.05f));
    star.setScale(scale, scale);

    // Random positions in the lower right corner
    float xPos = 1350 + std::rand() % 400; // Right-side horizontal position
    float yPos = 780 + std::rand() % 300; // Lower vertical spread
    star.setPosition(xPos, yPos);

    starSprites.push_back(star);
}




    }

    void handleEvents(sf::Event& event) override {
    if (event.type == sf::Event::TextEntered) {
        // Handle text input for username, email, password, and confirm password
        if (!usernameEntered) {
            // Handle username input
            if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                usernameEntered = true;
            } else if (event.text.unicode == 8 && !usernameInput.empty()) { // Backspace
                usernameInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Valid printable characters
                usernameInput += static_cast<char>(event.text.unicode);
            }
        } else if (!emailEntered) {
            // Handle email input
            if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                emailEntered = true;
            } else if (event.text.unicode == 8 && !emailInput.empty()) { // Backspace
                emailInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Valid printable characters
                emailInput += static_cast<char>(event.text.unicode);
            }
        } else if (!PasswordEntered) {
            // Handle email input
            if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                PasswordEntered = true;
            } else if (event.text.unicode == 8 && !PasswordInput.empty()) { // Backspace
                PasswordInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Valid printable characters
                PasswordInput += static_cast<char>(event.text.unicode);
            }
        
        }
  
else if (! ConfirmPasswordEntered) {
           
            if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                 ConfirmPasswordEntered = true;
            } else if (event.text.unicode == 8 && !ConfirmPasswordInput.empty()) { // Backspace
                ConfirmPasswordInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode > 31) { // Valid printable characters
                ConfirmPasswordInput += static_cast<char>(event.text.unicode);
            }
        
        } 

    } 
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f clickPos(event.mouseButton.x, event.mouseButton.y);

            // Handle "Sign Up" button click
            sf::FloatRect signUpButtonBounds(845, 825, 200, 58);
            if (signUpButtonBounds.contains(clickPos)) {
                // Validate inputs and check for password match
                if (!usernameInput.empty() && !emailInput.empty() && 
                    PasswordInput == ConfirmPasswordInput) {
                    changeState(LOGIN_SCREEN); // Proceed to the next screen
                } else {
                    // Show error message and set flags
                    showError = true;
                    passwordMismatch = PasswordInput != ConfirmPasswordInput;
                }
            }
        }
    }
}


    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 192, 203));

        // Title
        renderText("Fairyland Tetris", 790, 210, sf::Color::White, fontBindhu, 66, window);

        // Sign Up box
        sf::RectangleShape signUpBox(sf::Vector2f(740, 740));
        signUpBox.setFillColor(sf::Color(255, 192, 203));
        signUpBox.setOutlineColor(sf::Color::White);
        signUpBox.setOutlineThickness(3);
        signUpBox.setPosition(600, 290);
        window.draw(signUpBox);

        // Render stars on the left side
        for (auto& star : starSprites) {
            window.draw(star);
        }

        // Instruction text
        renderText("Sign up to enter the Fairyland", 630, 400, sf::Color::White, fontBindhu, 56, window);

        // Username label and box
        renderText("Username", 660, 510, sf::Color::White, fontTimes, 50, window);
        drawRoundedRectangle(872, 510, 350, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText(usernameInput.empty() ? "Username" : usernameInput, 920, 518, sf::Color(192, 192, 192), fontTimes, 33, window);

        // Email label and box
        renderText("Email", 660, 580, sf::Color::White, fontTimes, 50, window);
        drawRoundedRectangle(872, 580, 350, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText(emailInput.empty() ? "Email" : emailInput, 920, 588, sf::Color(192, 192, 192), fontTimes, 33, window);

        // Password label and box
        renderText("Password", 660, 650, sf::Color::White, fontTimes, 50, window);
        drawRoundedRectangle(872, 650, 350, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText(PasswordInput.empty() ? "Password" :  PasswordInput , 920, 658, sf::Color(192, 192, 192), fontTimes, 33, window);

      

        // Confirm Password label and box
        renderText("Confirm ", 660, 720, sf::Color::White, fontTimes, 50, window);
        drawRoundedRectangle(872, 720, 350, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText(ConfirmPasswordInput.empty() ? "Confirm Password" :  ConfirmPasswordInput , 920, 723, sf::Color(192, 192, 192), fontTimes, 33, window);

      

      
        // Error message
        if (showError) {
            renderText("Error: Invalid inputs or passwords do not match!", 690, 89, sf::Color::Red, fontTimes, 33, window);
        }

        // Sign-Up Button
        drawRoundedRectangle(845, 825, 200, 58, 15, sf::Color(255, 255, 255, 180), sf::Color(255, 192, 203), window);
        renderText("Sign Up", 867, 825, sf::Color(192, 192, 192), fontTimes, 45, window);

         // Instruction text
        renderText("Welcome to Fairyland!", 685, 898, sf::Color::White, fontBindhu, 60, window);

        window.display();
    }

    void handleTextInput(sf::Event& event, std::string& inputField, bool& moveToNext) {
        if (event.text.unicode == '\n' || event.text.unicode == '\r') {
            moveToNext = true;
        } else if (event.text.unicode == 8 && !inputField.empty()) { // Backspace
            inputField.pop_back();
        } else if (event.text.unicode < 128 && event.text.unicode > 31) {
            inputField += static_cast<char>(event.text.unicode);
        }
    }

 


void drawRoundedRectangle(float x, float y, float width, float height, float radius, 
                          sf::Color fillColor, sf::Color outlineColor, sf::RenderWindow& window) {
    const int pointCount = 30; // Number of points for smooth corners
    sf::VertexArray vertices(sf::TriangleFan);

    // Add points for the rounded rectangle
    float angleStep = 90.f / pointCount;

    // Top-left corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 180.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                y + radius + radius * std::sin(rad)), fillColor));
    }

    // Top-right corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 270.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                y + radius + radius * std::sin(rad)), fillColor));
    }

    // Bottom-right corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 0.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                y + height - radius + radius * std::sin(rad)), fillColor));
    }

    // Bottom-left corner
    for (int i = 0; i <= pointCount; ++i) {
        float angle = 90.f + i * angleStep;
        float rad = angle * (3.14159265f / 180.f);
        vertices.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                y + height - radius + radius * std::sin(rad)), fillColor));
    }

    // Draw the filled rounded rectangle
    window.draw(vertices);

    // Draw the outline separately
    if (outlineColor != sf::Color::Transparent) {
        sf::VertexArray outline(sf::LineStrip);

        // Add outline points for the rounded rectangle
        for (int i = 0; i <= pointCount; ++i) {
            float angle = 180.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                   y + radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 270.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                   y + radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 0.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + width - radius + radius * std::cos(rad), 
                                                   y + height - radius + radius * std::sin(rad)), outlineColor));
        }

        for (int i = 0; i <= pointCount; ++i) {
            float angle = 90.f + i * angleStep;
            float rad = angle * (3.14159265f / 180.f);
            outline.append(sf::Vertex(sf::Vector2f(x + radius + radius * std::cos(rad), 
                                                   y + height - radius + radius * std::sin(rad)), outlineColor));
        }

        outline.append(outline[0]); // Close the loop
        window.draw(outline);
    }
}


    void renderText(const std::string& text, float x, float y, const sf::Color& color, const sf::Font& font, unsigned int size, sf::RenderWindow& window) {
        sf::Text sfText;
        sfText.setFont(font);
        sfText.setString(text);
        sfText.setCharacterSize(size);
        sfText.setFillColor(color);
        sfText.setPosition(x, y);
        window.draw(sfText);
    }
};


/////////////////////////////////////////////////////////////
/////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////
//////////////////////////////////////////////////////////////
///////////////////////////////////////
// Game Won Screen
class GameWonScreenState : public State {
public:


    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                playerProfile.highestLevelUnlocked++;
                changeState(LEVELS_SCREEN);
            }
        }
    }

    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(100, 255, 100));
        renderText("You Won! Press Space to Continue", 400, 400, sf::Color::White, fontTimes, 40, window);
        window.display();
    }
};
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////
// Game Lost Screen
class GameLostScreenState : public State {
public:
    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                changeState(LEVELS_SCREEN);
            }
        }
    }

    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 100, 100));
        renderText("Game Over! Press Space to Retry", 400, 400, sf::Color::White, fontTimes, 40, window);
        window.display();
    }
};
/////////////////////////////////////
//////////////////////////////////////////
///////////////////////////////
/////////////////////
///////////////////////////////////////
class LevelsScreenState : public State {
private:
    int selectedLevel;
    std::function<void(int)> changeStateWithLevel;

    sf::Texture texture;
    sf::Sprite sprite;

    sf::Font fontBindhu;

    // Separate positions for beads and level texts
    std::vector<sf::Vector2f> beadPositions;
    std::vector<sf::Vector2f> levelTextPositions;

    // Texture and sprite for beads
    sf::Texture beadTexture;
    sf::Sprite beadSprite;

public:
    LevelsScreenState() {
        // Load background texture
        if (!texture.loadFromFile("assets/steps.jpg")) {
            std::cerr << "Unable to load background image!" << std::endl;
        }
        sprite.setTexture(texture);

        // Load font
        if (!fontBindhu.loadFromFile("assets/Bindhu.ttf")) {
            std::cerr << "Unable to load font!" << std::endl;
        }

        // Load bead texture
        if (!beadTexture.loadFromFile("assets/bead.png")) {
            std::cerr << "Unable to load bead image!" << std::endl;
        }
        beadSprite.setTexture(beadTexture);

        // Scale the bead sprite
        beadSprite.setScale(0.4f, 0.4f); // Adjust scale as needed

        // Define customizable bead positions
        beadPositions = {
            {1490.f, 15.f},  // Bead for Level 1
            {1345.f, 515.f},  // Bead for Level 2
            {1010.f, 150.f},  // Bead for Level 3
            {815.f, 815.f},   // Bead for Level 4
            {350.f, 375.f},   // Bead for Level 5
            {240.f, 883.f}    // Bead for Level 6
        };

        // Define customizable level text positions
        levelTextPositions = {
            {1585.f, 100.f},  // Text for Level 1
            {1435.f, 615.f},  // Text for Level 2
            {1100.f, 220.f},  // Text for Level 3
            {890.f, 910.f},   // Text for Level 4
            {440.f, 470.f},   // Text for Level 5
            {325.f, 980.f}    // Text for Level 6
        };
    }

    LevelsScreenState(std::function<void(int)> changeStateFunc)
        : selectedLevel(-1), changeStateWithLevel(changeStateFunc) {}

   void handleEvents(sf::Event& event)override {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Check if the mouse is clicking on any bead
            for (const auto& beadPosition : beadPositions) {
                sf::FloatRect beadBounds(beadPosition.x, beadPosition.y, 
                                         beadSprite.getGlobalBounds().width, 
                                         beadSprite.getGlobalBounds().height);
                if (beadBounds.contains(mousePos)) {
                    // Change state to GAME_RUNNING_SCREEN for any bead click
                    changeState(GAME_RUNNING_SCREEN);
                    return;
                }
            }
        }
    } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            selectedLevel = (selectedLevel - 1 + 6) % 6; // Navigate up through levels
        } else if (event.key.code == sf::Keyboard::Down) {
            selectedLevel = (selectedLevel + 1) % 6; // Navigate down through levels
        } else if (event.key.code == sf::Keyboard::Enter) {
            if (selectedLevel != -1) {
                changeState(GAME_RUNNING_SCREEN);
            }
        }
    }
}

    void update()override {
        // Placeholder for update logic if needed
    }

    void render(sf::RenderWindow& window)override {
        window.clear(sf::Color::Black);

        // Render background
        sf::Vector2u windowSize = window.getSize();
        sf::FloatRect bgBounds = sprite.getLocalBounds();
        sprite.setScale(
            static_cast<float>(windowSize.x) / bgBounds.width,
            static_cast<float>(windowSize.y) / bgBounds.height
        );
        window.draw(sprite);

        // Render beads and level texts
        for (int i = 0; i < beadPositions.size(); ++i) {
            if (i < beadPositions.size()) {
                // Render bead
                beadSprite.setPosition(beadPositions[i]);
                window.draw(beadSprite);
            }

            if (i < levelTextPositions.size()) {
                // Render level text
                sf::Text levelText;
                levelText.setFont(fontBindhu);
                levelText.setString( std::to_string(i + 1));
                levelText.setCharacterSize(90);
                levelText.setFillColor(sf::Color::White);
                levelText.setStyle(sf::Text::Bold);

                // Center text at the given position
                sf::FloatRect textBounds = levelText.getLocalBounds();
                levelText.setOrigin(textBounds.width / 2, textBounds.height / 2);
                levelText.setPosition(levelTextPositions[i]);

                window.draw(levelText);
            }
        }

        // Render title
        sf::Text titleText;
        titleText.setFont(fontBindhu);
        titleText.setString("Select a Level");
        titleText.setCharacterSize(100);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(100, 100);
        window.draw(titleText);

        window.display();
    }
};

//////////////////////////////////////////////////
////////////////////////////////
//////////////////////////////////////////////////////////
////////////////////////////////////////////
/////////////////////////////////////////////////////////////
////////////////////////
// Game running  Screen

class GameRunningScreenState : public State {
private:
    Tetris tetrisGame;
    std::function<void(GameState)> changeState;

public:
 GameRunningScreenState() 
        : changeState(nullptr) {} 


    GameRunningScreenState(std::function<void(GameState)> changeStateFunc) 
        : changeState(changeStateFunc) {}

    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) tetrisGame.moveBlockLeft();
            if (event.key.code == sf::Keyboard::Right) tetrisGame.moveBlockRight();
            if (event.key.code == sf::Keyboard::Down) tetrisGame.moveBlockDown();
            if (event.key.code == sf::Keyboard::Up) tetrisGame.rotateBlock();
        }
    }

    void update() override {
        tetrisGame.updateGameLogic();

        // Check for game over or win conditions
        if (tetrisGame.isGameOver()) {
            changeState(GAME_LOST_SCREEN);  // Transition to Game Over screen
        }
        else if (tetrisGame.isGameWon()) {
            changeState(GAME_WON_SCREEN);   // Transition to Game Won screen
        }
    }

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color(255, 192, 203)); // Set a background color
        tetrisGame.render(window); // Render the game board and active block
        window.display();
    }
};





////////////////////////////////////////
//////////////////////////////////////////////////
///////////////////////////////////
//////////////////////////////////////////////////
///////////////////////////////
////////////////////////////////////////////
//////////////////////////////
class ProfileScreenState : public State {
public:
    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            changeState(GAME_RUNNING_SCREEN);
        }
    }

    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color::Blue);
        renderText("Profile Screen", 400, 100, sf::Color::White, fontTimes, 50, window);
        renderText("Username: " + playerProfile.username, 400, 200, sf::Color::White, fontTimes, 30, window);
        renderText("Highest Level: " + std::to_string(playerProfile.highestLevelUnlocked), 400, 300, sf::Color::White, fontTimes, 30, window);
        renderText("Score: " + std::to_string(playerProfile.score), 400, 400, sf::Color::White, fontTimes, 30, window);
        renderText("Press Escape to return", 400, 500, sf::Color::White, fontTimes, 20, window);
        window.display();
    }
};
//////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////////////////////////////////////////
///////////////////////////////////////////////////
/////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////
/////////////////////////////

// Settings Screen State
class SettingsScreenState : public State {
public:
    void handleEvents(sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            changeState(GAME_RUNNING_SCREEN);
        }
    }

    void update() override {}

    void render(sf::RenderWindow& window) override {
        window.clear(sf::Color::Green);
        renderText("Settings Screen", 400, 100, sf::Color::White, fontTimes, 50, window);
        renderText("Adjust your settings here", 400, 200, sf::Color::White, fontTimes, 30, window);
        renderText("(Not implemented yet)", 400, 300, sf::Color::White, fontTimes, 20, window);
        renderText("Press Escape to return", 400, 400, sf::Color::White, fontTimes, 20, window);
        window.display();
    }
};



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
        currentStateInstance = std::make_unique<SignUpScreenState>();
        break;

        case GAME_RUNNING_SCREEN:
        currentStateInstance = std::make_unique<GameRunningScreenState>();
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
// Main Function
int main() {
    
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

   
    sf::RenderWindow window(
        sf::VideoMode(desktopMode.width * 0.8, desktopMode.height * 0.8), 
        getStateTitle(currentState),
        sf::Style::Default
    );

    // Center the window on the screen
    window.setPosition(sf::Vector2i(
        (desktopMode.width - window.getSize().x) / 2,
        (desktopMode.height - window.getSize().y) / 2
    ));

    // Load fonts
    if (!fontBindhu.loadFromFile("assets/Bindhu.ttf")) {
        std::cerr << "Unable to load Bindhu font!" << std::endl;
        return -1;
    }
    if (!fontTimes.loadFromFile("assets/times.ttf")) {
        std::cerr << "Unable to load Times font!" << std::endl;
        return -1;
    }

    // Initialize the application state
    changeState(SPLASH_SCREEN);

    // Main application loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // Close the window on close event
            }

            // Handle events in the current state
            if (currentStateInstance) {
                currentStateInstance->handleEvents(event);
            }
        }

        // Update and render the current state
        if (currentStateInstance) {
            currentStateInstance->update();
            currentStateInstance->render(window);
        }
    }

    return 0;
}
