#ifndef TETRIS_H
#define TETRIS_H

#include <raylib.h>

#define BACKGROUND_COLOR                                                       \
  (Color) { 2, 6, 23, 255 }
#define GRID_COLOR                                                             \
  (Color) { 30, 58, 138, 255 }
#define BACKGROUND_DARKER                                                      \
  (Color) { 23, 37, 84, 255 }
#define MY_BLUE                                                                \
  (Color) { 37, 99, 235, 255 }
#define MY_LIGHT_BLUE                                                          \
  (Color) { 96, 165, 250, 255 }
#define MY_ORANGE                                                              \
  (Color) { 245, 158, 11, 255 }
#define MY_RED                                                                 \
  (Color) { 220, 38, 38, 255 }
#define MY_GREEN                                                               \
  (Color) { 5, 150, 105, 255 }
#define MY_PURPLE                                                              \
  (Color) { 109, 40, 217, 255 }
#define MY_YELLOW                                                              \
  (Color) { 245, 158, 11, 255 }

#define X_SIZE 10
#define Y_SIZE 22
#define TETRI_SIZE 4
#define TYPE_COUNT 6
#define SQUARE_WIDTH 36

typedef enum SquareStatus { EMPTY, FILLED } SquareStatus;

typedef struct Square {
  SquareStatus status;
  Color color;
} Square;

typedef enum PieceType { I, T, O, S, Z, L, J } PieceType;

typedef struct Piece {
  int x;
  int y;
  PieceType type;
  SquareStatus grid[TETRI_SIZE][TETRI_SIZE];
  Color color;
} Piece;

typedef struct TetrisGame {
  Piece piece;
  Piece nextPiece;
  int gravitySpeed;
  int gravityCounter;
  int movementSpeed;
  int movementCounter;
  int linesCleared;
  int level;
  int score;
  bool paused;
  bool gameOver;
  Square squares[X_SIZE][Y_SIZE];
} TetrisGame;

void initGame(TetrisGame *game);
void updateGameState(TetrisGame *game);
#endif
