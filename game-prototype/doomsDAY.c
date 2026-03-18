/* This requires the installation of raylib, please follow the guide here: https://github.com/raysan5/raylib
 * This is purely for prototyping the game only; the game on the ATmega will not require this library
 *
 * Quick Start (for MacOS -- if brew is installed):
 * 1. brew install raylib
 * Then, to compile the .c file, do:
 * 2. eval cc yourgame.c $(pkg-config --libs --cflags raylib) -o YourGame
 * To run the obj file:
 * 3. ./YourGame (or whatever the object file name is)
 */

#include <stdio.h>
#include <string.h>
#include "raylib.h"


//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef struct Player {
    Vector2 position;
    int size;
    int health;
    Color color;
} Player;

typedef struct Enemy {
    Vector2 position;
    int size;
    bool isAlive;
    Color color;
} Enemy;

//----------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static int score = 0;
static bool gameOver = false;

static Vector2 playerPosition = {(float)screenWidth/2, (float)screenHeight/2};

Player player;



//----------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------

void InitGame() {

    gameOver = false;

    // Initialize Player
    player.position = playerPosition;
    player.size = 20;
    player.health = 3;
    player.color = BLACK;

}

void UpdateGame() {
    // TODO
    if (!gameOver) {

    }
}

void DrawGame() {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gameOver) {
            // Draw player
            DrawCircleV(player.position, player.size, player.color);
        } else {
            // char* endText = "Game Over :( Your Score is ";
            char gameOverText[50]; 
            snprintf(gameOverText, sizeof(gameOverText), "Game Over :( Your Score is %d.", score);
            DrawText(gameOverText, GetScreenWidth()/2 - MeasureText(gameOverText, 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }
    EndDrawing();
}

void UpdateDrawFrame() {
    UpdateGame();
    DrawGame();
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "doomsDAY");

    InitGame();

    SetTargetFPS(60);               // Set the game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and draw
        UpdateDrawFrame();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}