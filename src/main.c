#include "tetris.h"
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROJECT_NAME "tetris"

#define SCREEN_WIDTH X_SIZE *SQUARE_WIDTH + 200
#define SCREEN_HEIGHT Y_SIZE *SQUARE_WIDTH
#define LARGE_FONT 20
#define SMALL_FONT 12
#define GLSL_VERSION 330

// IMPERATIVE

void printStatus(TetrisGame *gamePtr) {
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      if (gamePtr->squares[i][j].status == FILLED) {
        printf("X");
      } else {
        printf("_");
      }
    }
    printf("\n");
  }
}
void drawSquare(int x, int y, Color color) {
  DrawRectangle(x * SQUARE_WIDTH, y * SQUARE_WIDTH, SQUARE_WIDTH, SQUARE_WIDTH,
                color);
}

void drawPiece(Piece *piece) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED) {
        Vector3 cubePosition = {.x = piece->position.x + i + 0.5f,
                                .y = -(piece->position.y + j) - 0.5f,
                                .z = -0.5f};
        Vector3 cubeSize = {.x = 1.0f, .y = 1.0f, .z = 1.0f};
        DrawCubeV(cubePosition, cubeSize, piece->color);
        DrawCubeWiresV(cubePosition, cubeSize, WHITE);
        // drawSquare(piece->x + i, piece->y + j, piece->color);
      }
    }
  }
}

void drawNextPiece(Piece *piece, int x, int y) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED) {
        DrawRectangle(x + (piece->position.x + i) * SQUARE_WIDTH,
                      y + (piece->position.y + j) * SQUARE_WIDTH, SQUARE_WIDTH,
                      SQUARE_WIDTH, piece->color);
      }
    }
  }
}

void drawGame(TetrisGame *gamePtr) {
  drawPiece(&gamePtr->piece);
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      if (gamePtr->squares[i][j].status == FILLED) {
        Vector3 cubePosition = {
            .x = (float)i + 0.5f, .y = (float)-j - 0.5f, .z = -0.5f};
        Vector3 cubeSize = {.x = 1.0f, .y = 1.0f, .z = 1.0f};
        DrawCubeV(cubePosition, cubeSize, gamePtr->squares[i][j].color);
        DrawCubeWiresV(cubePosition, cubeSize, WHITE);
      }
    }
  }
}

void drawGrid(void) {
  for (int i = -1; i < X_SIZE; ++i) {
    Vector3 startPosition = {.x = (float)(i + 1), .y = 0.0f, .z = 0.01f};
    Vector3 endPosition = {.x = (float)(i + 1), .y = -Y_SIZE, .z = 0.01f};
    DrawLine3D(startPosition, endPosition, LIGHTGRAY);
  }
  for (int j = -1; j < Y_SIZE; ++j) {
    Vector3 startPosition = {.x = 0.0f, .y = (float)-(j + 1), .z = 0.01f};
    Vector3 endPosition = {.x = (float)X_SIZE, (float)-(j + 1), 0.01f};
    DrawLine3D(startPosition, endPosition, LIGHTGRAY);
  }
}

void frame(TetrisGame *gamePtr, Camera3D *camera) {
  // UpdateCamera(camera, CAMERA_ORTHOGRAPHIC);

  BeginDrawing();
  {
    ClearBackground(BACKGROUND_COLOR);
    DrawRectangle(SCREEN_WIDTH - 200, 0, 200, SCREEN_HEIGHT, BACKGROUND_DARKER);
    int squareX = SCREEN_WIDTH - 200 + (200 - SQUARE_WIDTH * TETRI_SIZE) / 2;
    DrawRectangle(squareX, 120, SQUARE_WIDTH * TETRI_SIZE,
                  SQUARE_WIDTH * TETRI_SIZE, BACKGROUND_COLOR);

    drawNextPiece(&gamePtr->nextPiece, squareX - 3 * SQUARE_WIDTH + 10,
                  120 + SQUARE_WIDTH);

    DrawText("Next piece (TAB)", squareX, 120 + SQUARE_WIDTH * TETRI_SIZE + 20,
             16, WHITE);
    DrawLine(SCREEN_WIDTH - 200, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT, BLACK);

    BeginMode3D(*camera);
    {
      drawGame(gamePtr);
      drawGrid();
    }
    EndMode3D();

    DrawFPS(SCREEN_WIDTH - 80, 10);
    char buffer[40];
    sprintf(buffer, "Score: %d", gamePtr->score);
    DrawText(buffer, SCREEN_WIDTH - MeasureText(buffer, LARGE_FONT) - 10, 45,
             LARGE_FONT, WHITE);
    sprintf(buffer, "Level: %d", gamePtr->level);
    DrawText(buffer, SCREEN_WIDTH - MeasureText(buffer, LARGE_FONT) - 10,
             45 + LARGE_FONT + 10, LARGE_FONT, WHITE);

    if (gamePtr->paused) {
      char *paused = "PAUSED";
      DrawText(paused, SCREEN_WIDTH - MeasureText(paused, LARGE_FONT) - 50,
               SCREEN_HEIGHT - 50, LARGE_FONT, WHITE);
    }

    if (gamePtr->gameOver) {
      char *gameOverText = "GAME OVER";
      char *helpText = "Press space to start again";
      int smallTextWidth = MeasureText(helpText, SMALL_FONT);

      DrawRectangle(SCREEN_WIDTH / 2 - smallTextWidth / 2 - 32,
                    SCREEN_HEIGHT / 2 - 32, smallTextWidth + 64,
                    LARGE_FONT + SMALL_FONT + 74, BLACK);

      DrawText(gameOverText,
               SCREEN_WIDTH / 2 - MeasureText(gameOverText, LARGE_FONT) / 2,
               SCREEN_HEIGHT / 2, LARGE_FONT, RED);
      DrawText(helpText, SCREEN_WIDTH / 2 - smallTextWidth / 2,
               SCREEN_HEIGHT / 2 + LARGE_FONT + 10, SMALL_FONT, WHITE);
    }
  }
  EndDrawing();

  updateGameState(gamePtr);
}

int main(void) {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROJECT_NAME);

  Camera3D camera = {0};
  camera.position =
      (Vector3){.x = -10.25f, .y = -Y_SIZE / 2.0f + 10.25f, .z = 33.0f};
  camera.target = (Vector3){.x = X_SIZE - 2.0f, .y = -Y_SIZE / 2.0f, .z = 0.0f};
  camera.up = (Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor(); // Limit cursor to relative movement inside the window
  SetTargetFPS(60);

  TetrisGame game;
  initGame(&game);

  while (!WindowShouldClose()) {
    frame(&game, &camera);
  }

  CloseWindow();

  exit(EXIT_SUCCESS);
}
