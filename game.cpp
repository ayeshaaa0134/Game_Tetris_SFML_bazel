#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream> // For displaying the score

using namespace sf;

// Constants
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int BLOCK_SIZE = 30;

// Tetromino shapes
const int TETROMINOES[7][4][4] = {
    // I
    {
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    },
    // O
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // T
    {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // S
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // Z
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // J
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // L
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

// Tetromino structure
struct Tetromino {
    int shape[4][4];
    int x, y;

    Tetromino(int index) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                shape[i][j] = TETROMINOES[index][i][j];
            }
        }
        x = GRID_WIDTH / 2 - 2;
        y = 0;
    }

    // Rotate the tetromino clockwise
    void rotate() {
        int temp[4][4];
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                temp[i][j] = shape[3 - j][i];
            }
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                shape[i][j] = temp[i][j];
            }
        }
    }
};

// Function to check for collisions
bool isCollision(const Tetromino& tetromino, const std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (tetromino.shape[i][j] == 1) {
                int newX = tetromino.x + j;
                int newY = tetromino.y + i;

                if (newX < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT) {
                    return true; // Out of bounds
                }

                if (newY >= 0 && grid[newY][newX] == 1) {
                    return true; // Overlapping with a filled block
                }
            }
        }
    }
    return false;
}

// Lock tetromino into the grid
void lockTetromino(const Tetromino& tetromino, std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (tetromino.shape[i][j] == 1) {
                int newX = tetromino.x + j;
                int newY = tetromino.y + i;
                if (newY >= 0) {
                    grid[newY][newX] = 1;
                }
            }
        }
    }
}

// Clear full lines and update score
int clearLines(std::vector<std::vector<int>>& grid) {
    int linesCleared = 0;

    for (int y = GRID_HEIGHT - 1; y >= 0; --y) {
        bool fullLine = true;
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[y][x] == 0) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            ++linesCleared;
            for (int row = y; row > 0; --row) {
                for (int col = 0; col < GRID_WIDTH; ++col) {
                    grid[row][col] = grid[row - 1][col];
                }
            }
            for (int col = 0; col < GRID_WIDTH; ++col) {
                grid[0][col] = 0;
            }
            ++y;
        }
    }

    return linesCleared;
}

// Game Over Detection
bool isGameOver(const Tetromino& tetromino, const std::vector<std::vector<int>>& grid) {
    return isCollision(tetromino, grid);
}

// Draw grid
void drawGrid(RenderWindow& window, const std::vector<std::vector<int>>& grid) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            RectangleShape block(Vector2f(BLOCK_SIZE - 1, BLOCK_SIZE - 1));
            block.setPosition(x * BLOCK_SIZE, y * BLOCK_SIZE);
            block.setFillColor(grid[y][x] == 0 ? Color::Black : Color::Green);
            window.draw(block);
        }
    }
}

// Draw tetromino
void drawTetromino(RenderWindow& window, const Tetromino& tetromino) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (tetromino.shape[i][j] == 1) {
                RectangleShape block(Vector2f(BLOCK_SIZE - 1, BLOCK_SIZE - 1));
                block.setPosition((tetromino.x + j) * BLOCK_SIZE, (tetromino.y + i) * BLOCK_SIZE);
                block.setFillColor(Color::Green);
                window.draw(block);
            }
        }
    }
}

// Display score
void drawScore(RenderWindow& window, int score, Font& font) {
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::Green);
    scoreText.setPosition(10, 10);
    window.draw(scoreText);
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE), "Tetris - Tekken Theme");
    window.setFramerateLimit(60);

    std::vector<std::vector<int>> grid(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, 0));

    Tetromino currentTetromino(rand() % 7);
    Font font;
    font.loadFromFile("assets/font.ttf");

    int score = 0;

    Clock clock;
    float timer = 0;
    float delay = 0.5;

    bool gameOver = false;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        timer += deltaTime;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed && !gameOver) {
                Tetromino temp = currentTetromino;

                if (event.key.code == Keyboard::Left) temp.x -= 1;
                if (event.key.code == Keyboard::Right) temp.x += 1;
                if (event.key.code == Keyboard::Down) temp.y += 1;
                if (event.key.code == Keyboard::Up) temp.rotate();

                if (!isCollision(temp, grid)) {
                    currentTetromino = temp;
                }
            }
        }

        if (!gameOver && timer > delay) {
            Tetromino temp = currentTetromino;
            temp.y += 1;

            if (isCollision(temp, grid)) {
                lockTetromino(currentTetromino, grid);
                score += clearLines(grid) * 10;
                currentTetromino = Tetromino(rand() % 7);

                if (isGameOver(currentTetromino, grid)) {
                    gameOver = true;
                }
            } else {
                currentTetromino = temp;
            }

            timer = 0;
        }

        window.clear(Color::Green);

        drawGrid(window, grid);
        if (!gameOver) {
            drawTetromino(window, currentTetromino);
        }
        drawScore(window, score, font);

        if (gameOver) {
            Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("Game Over!");
            gameOverText.setCharacterSize(50);
            gameOverText.setFillColor(Color::Red);
            gameOverText.setPosition(50, 200);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}