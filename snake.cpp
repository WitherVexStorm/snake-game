#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

const int MAP_WIDTH = 600;
const int MAP_HEIGHT = 600;
const int MAP_PADDING = 4;
const int MENU_WIDTH = 300;
const int MENU_HEIGHT = MAP_HEIGHT;
const sf::Color MAP_FILL_COLOR = sf::Color::Transparent;
const sf::Color MAP_OUTLINE_COLOR = sf::Color::Yellow;
const sf::Color MENU_FILL_COLOR = sf::Color::Transparent;
const sf::Color MENU_OUTLINE_COLOR = sf::Color::Magenta;
const int WINDOW_WIDTH = MAP_PADDING + MAP_WIDTH + 2*MAP_PADDING + MENU_WIDTH + MAP_PADDING;
const int WINDOW_HEIGHT = MAP_PADDING + MAP_HEIGHT + MAP_PADDING;
const int MAP_ROWS = 20;
const int MAP_COLS = 20;
const int CELL_WIDTH = MAP_WIDTH / MAP_COLS;
const int CELL_HEIGHT = MAP_HEIGHT / MAP_ROWS;
const int SNAKE_SPEED = 1;
const int FOOD_RADIUS = CELL_WIDTH / 2;
const int FOOD_THICKNESS = 8;
const sf::Color FOOD_COLOR = sf::Color::Transparent;
const sf::Color FOOD_OUTLINE_COLOR = sf::Color::Red;
const int NORMAL_TEXT_SIZE = 20;
const int HEADER_TEXT_SIZE = 24;
const int MENU_HEADER_TEXT_SIZE = HEADER_TEXT_SIZE;
const int SCORE_TEXT_SIZE = NORMAL_TEXT_SIZE;
const int HIGH_SCORE_TEXT_SIZE = NORMAL_TEXT_SIZE;
const int PAUSE_TEXT_SIZE = NORMAL_TEXT_SIZE;
const int QUIT_TEXT_SIZE = NORMAL_TEXT_SIZE;
const int REPLAY_TEXT_SIZE = NORMAL_TEXT_SIZE;
const int GAME_OVER_TEXT_SIZE = 2 * HEADER_TEXT_SIZE;
const int BUTTON_HEIGHT = 20;
const int BUTTON_PADDING = 10;
const sf::Color BUTTON_OUTLINE_COLOR = sf::Color::Magenta;
const sf::Color PAUSE_ACTIVE_COLOR = sf::Color::Green;
const sf::Color PAUSE_INACTIVE_COLOR = sf::Color::Red;
const sf::Color QUIT_COLOR = sf::Color::Blue;
const sf::Color REPLAY_ACTIVE_COLOR = sf::Color::Green;
const sf::Color REPLAY_INACTIVE_COLOR = sf::Color::Red;
const sf::Color GAME_OVER_COLOR = sf::Color::Blue;
const sf::Color TEXT_COLOR = sf::Color::White;

struct cell
{
    bool isSnake;
    bool isFood;
} grid[MAP_ROWS][MAP_COLS];
int foodX, foodY;

struct gameStats
{
    int score;
    int highScore;
} stats;

class Snake
{
    public:
        enum direction
        {
            LEFT,
            UP,
            DOWN,
            RIGHT
        };

    private:
        static const int EYE_OFFSET = 4;
        static const int BODY_THICKNESS = 8;
        static const int BODY_WIDTH = CELL_WIDTH - 2 * BODY_THICKNESS;
        static const int BODY_HEIGHT = CELL_HEIGHT - 2 * BODY_THICKNESS;
        struct body
        {
            direction nextPiece;
            body * next;

            static int point2xcoord(int point)
            {
                return point * CELL_WIDTH + BODY_THICKNESS + MAP_PADDING;
            }

            static int point2ycoord(int point)
            {
                return point * CELL_HEIGHT + BODY_THICKNESS + MAP_PADDING;
            }

        } * tail;
        int x, y;
        sf::Color bodyColor;
        sf::Color eyeColor;
        sf::Color fillColor;
        direction head_direction;

    public:
        Snake(int _x = 1, int _y = 1, int _length = 3, direction _dir = RIGHT, sf::Color _bodyColor = sf::Color::Green, sf::Color _eyeColor = sf::Color::White, sf::Color _fillColor = sf::Color::Transparent)
        {
            x = _x;
            y = _y;
            eyeColor = _eyeColor;
            bodyColor = _bodyColor;
            fillColor = _fillColor;
            grid[_y][_x].isSnake = true;
            head_direction = _dir;
            body * temp = tail = new body{ _dir, nullptr };
            while (--_length)
            {
                switch(_dir)
                {
                    case LEFT:
                        _x = (_x - 1 + MAP_COLS) % MAP_COLS;
                        break;
                    case UP:
                        _y = (_y - 1 + MAP_ROWS) % MAP_ROWS;
                        break;
                    case DOWN:
                        _y = (_y + 1) % MAP_ROWS;
                        break;
                    case RIGHT:
                        _x = (_x + 1) % MAP_COLS;
                        break;
                }
                grid[_y][_x].isSnake = true;
                temp->next = new body{ _dir, nullptr };
                temp = temp->next;
            }
        }

        direction getDirection()
        {
            return head_direction;
        }

        direction oppositeDirection(direction _dir)
        {
            switch (_dir)
            {
                case LEFT:
                    return RIGHT;
                case UP:
                    return DOWN;
                case DOWN:
                    return UP;
                case RIGHT:
                    return LEFT;
            }
        }

        void draw(sf::RenderWindow &window)
        {
            body * temp = tail;
            int tempX = x;
            int tempY = y;
            while (temp->next != nullptr)
            {
                drawBody(window, tempX, tempY);
                movePoint(tempX, tempY, temp->nextPiece);
                temp = temp->next;
            }
            drawBody(window, tempX, tempY);
            drawHead(window, tempX, tempY, temp->nextPiece);
        }

        static int point2xcoord(int point, int modifier = 0)
        {
            return point * CELL_WIDTH + BODY_THICKNESS + MAP_PADDING - BODY_THICKNESS + EYE_OFFSET + modifier * BODY_WIDTH;
        }

        static int point2ycoord(int point, int modifier = 0)
        {
            return point * CELL_HEIGHT + BODY_THICKNESS + MAP_PADDING - BODY_THICKNESS + EYE_OFFSET + modifier * BODY_HEIGHT;
        }

        void drawBody(sf::RenderWindow &window, int _x, int _y)
        {
            sf::RectangleShape body(sf::Vector2f(BODY_WIDTH, BODY_HEIGHT));
            body.setPosition(body::point2xcoord(_x), body::point2ycoord(_y));
            body.setFillColor(fillColor);
            body.setOutlineColor(bodyColor);
            body.setOutlineThickness(BODY_THICKNESS);
            window.draw(body);
        }

        void drawHead(sf::RenderWindow &window, int _x, int _y, direction _dir)
        {
            sf::CircleShape eye1(4);
            eye1.setFillColor(eyeColor);
            eye1.setPosition(point2xcoord(_x, _dir/2), point2ycoord(_y, _dir/2));
            window.draw(eye1);

            sf::CircleShape eye2(4);
            eye2.setFillColor(eyeColor);
            eye2.setPosition(point2xcoord(_x, _dir%2), point2ycoord(_y, 1-_dir%2));
            window.draw(eye2);
        }

        void movePoint(int &x, int &y, direction _dir)
        {
            switch (_dir)
            {
                case LEFT:
                    x = (x - 1 + MAP_COLS) % MAP_COLS;
                    break;
                case UP:
                    y = (y - 1 + MAP_ROWS) % MAP_ROWS;
                    break;
                case DOWN:
                    y = (y + 1) % MAP_ROWS;
                    break;
                case RIGHT:
                    x = (x + 1) % MAP_COLS;
                    break;
            }
        }

        int move(direction _dir)
        {
            // std::cout << "Moving snake.\n";
            int tempX = x;
            int tempY = y;
            body * temp = tail;
            while (temp->next != nullptr)
            {
                // std::cout << tempX << ", " << tempY << "\n";
                movePoint(tempX, tempY, temp->nextPiece);
                temp = temp->next;
            }
            // std::cout << tempX << ", " << tempY << "\n";
            temp->nextPiece = _dir; // Move head
            temp->next = new body{ head_direction = _dir, nullptr };
            movePoint(tempX, tempY, _dir);
            // std::cout << tempX << ", " << tempY << " -> HEAD(" << grid[tempY][tempX].isSnake << " - " << grid[tempY][tempX].isFood << ")\n";
            if (grid[tempY][tempX].isFood)
            {
                grid[tempY][tempX].isFood = false; // Eat food
                grid[y][x].isSnake = true;         // DON'T move tail
            }
            else // Move tail
            {
                grid[y][x].isSnake = false;
                body * oldTail = tail;
                movePoint(x, y, oldTail->nextPiece);
                // std::cout << x << ", " << y << " -> TAIL(" << grid[y][x].isSnake << ")\n";
                tail = tail->next;
                delete oldTail;
            }
            if (grid[tempY][tempX].isSnake) // If hit itself
            {
                return -1; // Game over
            }
            grid[tempY][tempX].isSnake = true; // Move head
            return 0;
        }

        ~Snake()
        {
            body * temp = tail;
            while (temp != nullptr)
            {
                body * old = temp;
                temp = temp->next;
                delete old;
            }
        }
};

void generateFood()
{
    do
    {
        foodX = rand() % MAP_COLS;
        foodY = rand() % MAP_ROWS;
    } while (grid[foodY][foodX].isSnake);
    grid[foodY][foodX].isFood = true;
}

int point2foodxcoord(int point)
{
    return point * CELL_WIDTH + FOOD_THICKNESS + MAP_PADDING;
}

int point2foodycoord(int point)
{
    return point * CELL_HEIGHT + FOOD_THICKNESS + MAP_PADDING;
}

void setupFont(sf::Font &font)
{
    if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf"))
    {
        std::cerr << "Failed to load font.\n";
        exit(1);
    }
}

void setText(sf::RenderWindow& window, sf::Text &text, sf::Font &font, std::string str, int size, sf::Color color, int centerX, int centerY, sf::Uint32 style = sf::Text::Regular)
{
    int x = centerX - str.length() * size / 3.5;
    int y = centerY - size / 1.8;
    // std::cout << str << " at " << x << ", " << y << "\n";
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setStyle(style);
    text.setPosition(x, y);
    window.draw(text);
}

int main()
{
    srand(time(NULL));
    sf::Font font;
    setupFont(font);

    // Create an SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game");

    sf::Clock clock;
    sf::Time elapsedTime = sf::Time::Zero;
    sf::Time timePerFrame = sf::seconds(1.0/SNAKE_SPEED);

    Snake * snake = new Snake();
    Snake::direction dir = Snake::RIGHT;
    bool isGameOver = false;
    bool isPaused = false;

    generateFood();

    sf::Text menuHeaderText;
    sf::Text scoreText;
    sf::Text highScoreText;
    sf::Text pauseText;
    sf::Text quitText;
    sf::Text replayText;
    sf::Text gameOverText;

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // Close the window
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Left:
                        if (snake->getDirection() != Snake::RIGHT)
                        {
                            dir = Snake::LEFT;
                        }
                        break;
                    case sf::Keyboard::Right:
                        if (snake->getDirection() != Snake::LEFT)
                        {
                            dir = Snake::RIGHT;
                        }
                        break;
                    case sf::Keyboard::Up:
                        if (snake->getDirection() != Snake::DOWN)
                        {
                            dir = Snake::UP;
                        }
                        break;
                    case sf::Keyboard::Down:
                        if (snake->getDirection() != Snake::UP)
                        {
                            dir = Snake::DOWN;
                        }
                        break;
                    case sf::Keyboard::Space:
                        // Pause the game
                        isPaused = !isPaused;
                        break;
                    case sf::Keyboard::R:
                        // Replay the game
                        delete snake;
                        snake = new Snake();
                        dir = Snake::RIGHT;
                        isGameOver = false;
                        stats.score = 0;
                        generateFood();
                        break;
                    case sf::Keyboard::Q:
                        // Close the window
                        window.close();
                        break;
                }
            }
        }

        elapsedTime += clock.restart();
        while (elapsedTime > timePerFrame)
        {
            elapsedTime -= timePerFrame;

            // Update the game
            if (!isPaused && !isGameOver && snake->move(dir) == -1)
            {
                // Game over
                std::cout << "Game over.\n";
                isGameOver = true;
                if (stats.score > stats.highScore)
                {
                    stats.highScore = stats.score;
                }
            }
            if (!grid[foodY][foodX].isFood)
            {
                stats.score += 1;
                generateFood();
            }
        }

        // Clear the window
        window.clear();

        // Draw map border
        sf::RectangleShape mapborder(sf::Vector2f(MAP_WIDTH, MAP_HEIGHT));
        mapborder.setPosition(MAP_PADDING, MAP_PADDING);
        mapborder.setFillColor(sf::Color::Transparent);
        mapborder.setOutlineColor(sf::Color::Yellow);
        mapborder.setOutlineThickness(MAP_PADDING);
        window.draw(mapborder);

        // Draw menu border
        sf::RectangleShape menuborder(sf::Vector2f(MENU_WIDTH, MENU_HEIGHT));
        menuborder.setPosition(MAP_PADDING + MAP_WIDTH + 2*MAP_PADDING, MAP_PADDING);
        menuborder.setFillColor(MENU_FILL_COLOR);
        menuborder.setOutlineColor(MENU_OUTLINE_COLOR);
        menuborder.setOutlineThickness(MAP_PADDING);
        window.draw(menuborder);

        // Draw food
        sf::CircleShape food(FOOD_RADIUS - FOOD_THICKNESS);
        food.setFillColor(FOOD_COLOR);
        food.setPosition(point2foodxcoord(foodX), point2foodycoord(foodY));
        food.setOutlineColor(FOOD_OUTLINE_COLOR);
        food.setOutlineThickness(FOOD_THICKNESS);
        window.draw(food);
        // std::cout << "Food at " << foodX << ", " << foodY << ", coords " << point2foodxcoord(foodX) << ", " << point2foodycoord(foodY) << "\n";

        // Draw snake
        snake->draw(window);

        // Draw menu buttons
        sf::RectangleShape pauseTextButton(sf::Vector2f(MENU_WIDTH/2, BUTTON_HEIGHT + BUTTON_PADDING * 2));
        pauseTextButton.setPosition(MAP_PADDING + MAP_WIDTH + 2*MAP_PADDING + MENU_WIDTH/4, MAP_PADDING + MENU_HEIGHT * 0.7 - BUTTON_HEIGHT/2 - BUTTON_PADDING);
        pauseTextButton.setFillColor(isPaused ? PAUSE_ACTIVE_COLOR : PAUSE_INACTIVE_COLOR);
        pauseTextButton.setOutlineColor(BUTTON_OUTLINE_COLOR);
        pauseTextButton.setOutlineThickness(MAP_PADDING);
        window.draw(pauseTextButton);

        sf::RectangleShape quitTextButton(sf::Vector2f(MENU_WIDTH/2, BUTTON_HEIGHT + BUTTON_PADDING * 2));
        quitTextButton.setPosition(MAP_PADDING + MAP_WIDTH + 2*MAP_PADDING + MENU_WIDTH/4, MAP_PADDING + MENU_HEIGHT * 0.8 - BUTTON_HEIGHT/2 - BUTTON_PADDING);
        quitTextButton.setFillColor(QUIT_COLOR);
        quitTextButton.setOutlineColor(BUTTON_OUTLINE_COLOR);
        quitTextButton.setOutlineThickness(MAP_PADDING);
        window.draw(quitTextButton);

        sf::RectangleShape replayTextButton(sf::Vector2f(MENU_WIDTH/2, BUTTON_HEIGHT + BUTTON_PADDING * 2));
        replayTextButton.setPosition(MAP_PADDING + MAP_WIDTH + 2*MAP_PADDING + MENU_WIDTH/4, MAP_PADDING + MENU_HEIGHT * 0.9 - BUTTON_HEIGHT/2 - BUTTON_PADDING);
        replayTextButton.setFillColor(isGameOver ? REPLAY_ACTIVE_COLOR : REPLAY_INACTIVE_COLOR);
        replayTextButton.setOutlineColor(BUTTON_OUTLINE_COLOR);
        replayTextButton.setOutlineThickness(MAP_PADDING);
        window.draw(replayTextButton);

        // Draw menu text
        setText(window, menuHeaderText, font, "Menu", MENU_HEADER_TEXT_SIZE, TEXT_COLOR, MAP_PADDING + MAP_WIDTH + 2 * MAP_PADDING + MENU_WIDTH / 2, MAP_PADDING + MENU_HEIGHT * 0.05, sf::Text::Bold | sf::Text::Underlined);
        setText(window, scoreText, font, "Score: " + std::to_string(stats.score), SCORE_TEXT_SIZE, TEXT_COLOR, MAP_PADDING + MAP_WIDTH + 2 * MAP_PADDING + MENU_WIDTH / 4, MAP_PADDING + MENU_HEIGHT * 0.3);
        setText(window, highScoreText, font, "High Score: " + std::to_string(stats.highScore), HIGH_SCORE_TEXT_SIZE, TEXT_COLOR, MAP_PADDING + MAP_WIDTH + 2 * MAP_PADDING + MENU_WIDTH / 4, MAP_PADDING + MENU_HEIGHT * 0.4);
        setText(window, pauseText, font, "Space: Pause", PAUSE_TEXT_SIZE, TEXT_COLOR, MAP_PADDING + MAP_WIDTH + 2 * MAP_PADDING + MENU_WIDTH / 2, MAP_PADDING + MENU_HEIGHT * 0.7);
        setText(window, quitText, font, "Q: Quit", QUIT_TEXT_SIZE, TEXT_COLOR, MAP_PADDING + MAP_WIDTH + 2 * MAP_PADDING + MENU_WIDTH / 2, MAP_PADDING + MENU_HEIGHT * 0.8);
        setText(window, replayText, font, "R: Replay", REPLAY_TEXT_SIZE, TEXT_COLOR, MAP_PADDING + MAP_WIDTH + 2 * MAP_PADDING + MENU_WIDTH / 2, MAP_PADDING + MENU_HEIGHT * 0.9);

        if (isGameOver)
        {
            setText(window, gameOverText, font, "Game Over", GAME_OVER_TEXT_SIZE, QUIT_COLOR, MAP_PADDING + MAP_WIDTH/2, MAP_PADDING + MAP_HEIGHT/2, sf::Text::Bold);
            window.draw(gameOverText);
        }

        // Display the contents of the window
        window.display();
    }
    delete snake;

    return 0;
}
