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
        drawSquare(piece->x + i, piece->y + j, piece->color);
      }
    }
  }
}

void drawNextPiece(Piece *piece, int x, int y) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED) {
        DrawRectangle(x + (piece->x + i) * SQUARE_WIDTH,
                      y + (piece->y + j) * SQUARE_WIDTH, SQUARE_WIDTH,
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
        drawSquare(i, j, gamePtr->squares[i][j].color);
      }
    }
  }
}

void drawGrid(void) {
  for (int i = 0; i < X_SIZE; ++i) {
    DrawLine((i + 1) * SQUARE_WIDTH, 0, (i + 1) * SQUARE_WIDTH, SCREEN_HEIGHT,
             GRID_COLOR);
  }
  for (int j = 0; j < Y_SIZE; ++j) {
    DrawLine(0, (j + 1) * SQUARE_WIDTH, X_SIZE * SQUARE_WIDTH,
             (j + 1) * SQUARE_WIDTH, GRID_COLOR);
  }
}

void frame(TetrisGame *gamePtr) {
  BeginDrawing();
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
  drawGame(gamePtr);
  drawGrid();
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
  EndDrawing();

  updateGameState(gamePtr);
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROJECT_NAME);

  SetTargetFPS(60);

  TetrisGame game;
  initGame(&game);

  while (!WindowShouldClose()) {
    frame(&game);
  }

  CloseWindow();

  exit(EXIT_SUCCESS);
}
