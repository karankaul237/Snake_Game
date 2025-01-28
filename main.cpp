#include <iostream>
#include "raylib.h"
#include <deque>
#include <raymath.h>
using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 20;
int offset = 70;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake {
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};  
    bool addSegment = false;

    void Draw() {
        for (long long unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = {offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update() {
        body.push_front(Vector2Add(body[0], direction));

        if (addSegment) {
            addSegment = false;  
        } else {
            body.pop_back(); 
        }
    }

    void Reset() {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};  
        direction = {1, 0};  
    }
};

class Food {
public:
    Vector2 position;
    Texture2D normalTexture;

    Food(deque<Vector2> snakeBody) {
        Image normalImage = LoadImage("Untitled.png");
        normalTexture = LoadTextureFromImage(normalImage);
        UnloadImage(normalImage);

        position = GenerateRandomPos(snakeBody);
    }

    ~Food() {
        UnloadTexture(normalTexture);
    }

    void Draw() {
        DrawTexture(normalTexture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = false;
    int score = 0; 

    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        if (running) {
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
            snake.Update();  
        }
    }

    void UpdateScore(int points) {
        score += points;  
    }

    void ResetScore() {
        score = 0;  
    }

    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            UpdateScore(1);  
        }
    }

    void CheckCollisionWithEdges() {
        if (snake.body[0].x == 0 || snake.body[0].x == cellCount || snake.body[0].y == 0 || snake.body[0].y == cellCount) {
            GameOver(); 
        }
    }

    void GameOver() {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
    }

    void CheckCollisionWithTail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) {
            GameOver();  
        }
    }
};

int main() {
    cout << "Starting the game..." << endl;
    InitWindow(2 * offset + (cellCount * cellSize), 2 * offset + (cellCount * cellSize), "Retro Snake Game");
    SetTargetFPS(60);

    Game game = Game(); 

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        if (eventTriggered(0.2)){
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = {0, -1};
            //game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = {0, 1};
            //game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0};
            //game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = {1, 0};
            //game.running = true;
        }

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)(cellCount * cellSize) + 10, (float)(cellCount * cellSize) + 10}, 5, darkGreen);
        DrawText("Retro Snake Game", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellCount * cellSize + 10, 40, darkGreen);

        if (!game.running) {
            int windowWidth = GetScreenWidth();
            int windowHeight = GetScreenHeight();
            const char* scoreText = TextFormat("You scored: %i", game.score);
            const char* gameOverText = "GAME OVER";
            const char* restartText = "Press ENTER to restart";

            int scoreTextWidth = MeasureText(scoreText, 30);
            int gameOverTextWidth = MeasureText(gameOverText, 30);
            int restartTextWidth = MeasureText(restartText, 30);

            int scoreTextX = (windowWidth - scoreTextWidth) / 2;
            int gameOverTextX = (windowWidth - gameOverTextWidth) / 2;
            int restartTextX = (windowWidth - restartTextWidth) / 2;
            int centerY = (windowHeight - 100) / 2; 

            DrawText(gameOverText, gameOverTextX, centerY - 40, 30, RED);
            DrawText(scoreText, scoreTextX, centerY, 30, darkGreen);
            DrawText(restartText, restartTextX, centerY + 40, 30, darkGreen);
        }
          
        game.Draw(); 

        if (!game.running && IsKeyPressed(KEY_ENTER)) {
            game.running = true;
            game.snake.Reset();
            game.food.position = game.food.GenerateRandomPos(game.snake.body);
            game.ResetScore(); 
        } 

        EndDrawing();
    }

    CloseWindow();
    return 0;
}