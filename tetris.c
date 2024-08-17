#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FUNCTIONAL DOMAIN LOGIC
#define PROJECT_NAME "tetris"

#define X_SIZE 10
#define Y_SIZE 22

#define SQUARE_WIDTH 36

#define SCREEN_WIDTH X_SIZE *SQUARE_WIDTH + 200
#define SCREEN_HEIGHT Y_SIZE *SQUARE_WIDTH
#define TETRI_SIZE 4
#define TYPE_COUNT 6
#define LARGE_FONT 20
#define SMALL_FONT 12

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

typedef enum SquareStatus { EMPTY, FILLED } SquareStatus;

typedef struct Square {
  SquareStatus status;
  Color color;
} Square;

static Square squares[X_SIZE][Y_SIZE];

typedef enum PieceType { I, T, O, S, Z, L, J } PieceType;

typedef struct Piece {
  int x;
  int y;
  PieceType type;
  SquareStatus grid[TETRI_SIZE][TETRI_SIZE];
  Color color;
} Piece;

static Piece piece;
static Piece nextPiece;
static int gravitySpeed;
static int gravityCounter;
static int movementSpeed;
static int movementCounter;
static int linesCleared;
static int level;
static int score;

static bool paused;
static bool gameOver;

void initPiece(void) {
  piece = nextPiece;
  int val = GetRandomValue(0, TYPE_COUNT);

  nextPiece.x = (int)floor((double)X_SIZE / 2 - 2);
  nextPiece.y = 0;
  nextPiece.type = val;

  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      nextPiece.grid[i][j] = EMPTY;
    }
  }
  switch (nextPiece.type) {
  case J:
    nextPiece.grid[0][0] = FILLED;
    nextPiece.grid[0][1] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.grid[2][1] = FILLED;
    nextPiece.color = MY_BLUE;
    break;
  case L:
    nextPiece.grid[0][1] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.grid[2][1] = FILLED;
    nextPiece.grid[2][0] = FILLED;
    nextPiece.color = MY_ORANGE;
    break;
  case O:
    nextPiece.grid[1][0] = FILLED;
    nextPiece.grid[2][0] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.grid[2][1] = FILLED;
    nextPiece.color = MY_YELLOW;
    break;
  case T:
    nextPiece.grid[1][0] = FILLED;
    nextPiece.grid[0][1] = FILLED;
    nextPiece.grid[2][1] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.color = MY_PURPLE;
    break;
  case Z:
    nextPiece.grid[0][0] = FILLED;
    nextPiece.grid[1][0] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.grid[2][1] = FILLED;
    nextPiece.color = MY_RED;
    break;
  case S:
    nextPiece.grid[2][0] = FILLED;
    nextPiece.grid[1][0] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.grid[0][1] = FILLED;
    nextPiece.color = MY_GREEN;
    break;
  case I:
    nextPiece.grid[0][1] = FILLED;
    nextPiece.grid[1][1] = FILLED;
    nextPiece.grid[2][1] = FILLED;
    nextPiece.grid[3][1] = FILLED;
    nextPiece.color = MY_LIGHT_BLUE;
    nextPiece.y = -1;
    break;
  }
}

void initGame(void) {
  gravitySpeed = 48;
  gravityCounter = 0;
  movementSpeed = 6;
  movementCounter = 0;

  score = 0;
  linesCleared = 0;
  level = 1;
  paused = false;
  gameOver = false;
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      squares[i][j].status = EMPTY;
      squares[i][j].color = BACKGROUND_COLOR;
    }
  }
  initPiece(); // Twice to initalize next piece
  initPiece();
}

void rotateCentered(void) {
  SquareStatus aux = piece.grid[1][0];
  piece.grid[1][0] = piece.grid[0][1];
  piece.grid[0][1] = piece.grid[1][2];
  piece.grid[1][2] = piece.grid[2][1];
  piece.grid[2][1] = aux;

  aux = piece.grid[0][0];
  piece.grid[0][0] = piece.grid[0][2];
  piece.grid[0][2] = piece.grid[2][2];
  piece.grid[2][2] = piece.grid[2][0];
  piece.grid[2][0] = aux;
}

void rotateI(void) {
  SquareStatus aux = piece.grid[0][1];
  piece.grid[0][1] = piece.grid[1][3];
  piece.grid[1][3] = piece.grid[3][2];
  piece.grid[3][2] = piece.grid[2][0];
  piece.grid[2][0] = aux;

  aux = piece.grid[3][1];
  piece.grid[3][1] = piece.grid[1][0];
  piece.grid[1][0] = piece.grid[0][2];
  piece.grid[0][2] = piece.grid[2][3];
  piece.grid[2][3] = aux;

  aux = piece.grid[1][1];
  piece.grid[1][1] = piece.grid[1][2];
  piece.grid[1][2] = piece.grid[2][2];
  piece.grid[2][2] = piece.grid[2][1];
  piece.grid[2][1] = aux;
}

void rotate(void) {
  switch (piece.type) {
  case T:
  case L:
  case J:
  case S:
  case Z:
    rotateCentered();
    break;
  case I:
    rotateI();
    break;
  case O:
    break;
  }
}

bool shouldStop(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if ((piece.grid[i][j] == FILLED &&
           squares[piece.x + i][piece.y + j].status == FILLED) ||
          (piece.grid[i][j] == FILLED && (piece.y + j) == Y_SIZE)) {
        return true;
      }
    }
  }
  return false;
}

void clearLines(void) {
  int numLinesCleared = 0;
  for (int j = 0; j < Y_SIZE; ++j) {
    bool clear = true;
    for (int i = 0; i < X_SIZE; ++i) {
      if (squares[i][j].status == EMPTY) {
        clear = false;
      }
    }
    if (clear) {
      numLinesCleared += 1;
      linesCleared += 1;
      for (int k = j - 1; k > 0; --k) {
        for (int i = 0; i < X_SIZE; ++i) {
          squares[i][k + 1].status = squares[i][k].status;
        }
      }
    }
  }
  if (linesCleared > 10) {
    linesCleared = linesCleared - 10;
    level += 1;
    gravitySpeed -= 5;
  }
  switch (numLinesCleared) {
  case 1:
    score += 40 * level;
    break;
  case 2:
    score += 100 * level;
    break;
  case 3:
    score += 300 * level;
    break;
  case 4:
    score += 1200 * level;
    break;
  default:
    break;
  }
}

void fillStatuses(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece.grid[i][j] == FILLED) {
        squares[piece.x + i][piece.y + j - 1].status = FILLED;
        squares[piece.x + i][piece.y + j - 1].color = piece.color;
      }
    }
  }
}

bool isGameOver(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece.grid[i][j] == FILLED && piece.y + j == 0) {
        return true;
      }
    }
  }
  return false;
}

bool canGoRight(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece.grid[i][j] == FILLED &&
          (piece.x + i == X_SIZE - 1 ||
           squares[piece.x + i + 1][piece.y + j].status == FILLED)) {
        return false;
      }
    }
  }
  return true;
}

bool canGoLeft(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece.grid[i][j] == FILLED &&
          (piece.x + i == 0 ||
           squares[piece.x + i - 1][piece.y + j].status == FILLED)) {
        return false;
      }
    }
  }
  return true;
}

bool canRotate(void) {
  rotate();
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece.grid[i][j] == FILLED &&
          ((piece.x + i < 0) || piece.x + i >= X_SIZE || piece.y + j < 0 ||
           piece.y + j >= Y_SIZE ||
           squares[piece.x + i][piece.y + j].status == FILLED)) {
        rotate();
        rotate();
        rotate(); // rotate back to previous point LOL
        return false;
      }
    }
  }
  rotate();
  rotate();
  rotate();
  return true;
}

void updateGameState(void) {
  if (!paused && !gameOver) {
    bool moved = false;
    if ((IsKeyPressed(KEY_RIGHT) ||
         (IsKeyDown(KEY_RIGHT) && movementCounter > movementSpeed)) &&
        canGoRight()) {
      piece.x += 1;
      movementCounter = 0;
      moved = true;
    } else if ((IsKeyPressed(KEY_LEFT) ||
                (IsKeyDown(KEY_LEFT) && movementCounter > movementSpeed)) &&
               canGoLeft()) {
      piece.x -= 1;
      movementCounter = 0;
      moved = true;
    } else if (IsKeyDown(KEY_DOWN)) {
      gravityCounter += gravitySpeed / 2;
      moved = true;
    } else if (IsKeyPressed(KEY_UP) && canRotate()) {
      rotate();
      moved = true;
    } else if (IsKeyPressed(KEY_SPACE)) {
      while (!shouldStop()) {
        piece.y += 1;
      }
    }
    if (movementCounter > movementSpeed) {
      movementCounter = 0;
    } else {
      ++movementCounter;
    }

    if (shouldStop()) {
      fillStatuses();
      clearLines();

      gameOver = isGameOver();
      if (!gameOver) {
        initPiece(); // val);
      }
    }
    if (!moved) {
      gravityCounter += 1;
    }

    if (gravityCounter > gravitySpeed) {
      gravityCounter = 0;
      piece.y += 1;
    }
  }
  if (IsKeyPressed(KEY_P)) {
    paused = !paused;
  }
  if (gameOver && IsKeyPressed(KEY_SPACE)) {
    initGame();
  }
}

// IMPERATIVE

void printStatus(void) {
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      if (squares[i][j].status == FILLED) {
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

void drawPiece(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece.grid[i][j] == FILLED) {
        drawSquare(piece.x + i, piece.y + j, piece.color);
      }
    }
  }
}

void drawNextPiece(int x, int y) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (nextPiece.grid[i][j] == FILLED) {
        DrawRectangle(x + (nextPiece.x + i) * SQUARE_WIDTH,
                      y + (nextPiece.y + j) * SQUARE_WIDTH, SQUARE_WIDTH,
                      SQUARE_WIDTH, nextPiece.color);
      }
    }
  }
}

void drawGame(void) {
  drawPiece();
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      if (squares[i][j].status == FILLED) {
        drawSquare(i, j, squares[i][j].color);
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

void frame(void) {
  BeginDrawing();
  ClearBackground(BACKGROUND_COLOR);
  DrawRectangle(SCREEN_WIDTH - 200, 0, 200, SCREEN_HEIGHT, BACKGROUND_DARKER);
  int squareX = SCREEN_WIDTH - 200 + (200 - SQUARE_WIDTH * TETRI_SIZE) / 2;
  DrawRectangle(squareX, 120, SQUARE_WIDTH * TETRI_SIZE,
                SQUARE_WIDTH * TETRI_SIZE, BACKGROUND_COLOR);

  drawNextPiece(squareX - 3 * SQUARE_WIDTH + 10, 120 + SQUARE_WIDTH);

  DrawText("Next piece (TAB)", squareX, 120 + SQUARE_WIDTH * TETRI_SIZE + 20,
           16, WHITE);
  DrawLine(SCREEN_WIDTH - 200, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT, BLACK);
  drawGame();
  drawGrid();
  DrawFPS(SCREEN_WIDTH - 80, 10);
  char buffer[40];
  sprintf(buffer, "Score: %d", score);
  DrawText(buffer, SCREEN_WIDTH - MeasureText(buffer, LARGE_FONT) - 10, 45,
           LARGE_FONT, WHITE);
  sprintf(buffer, "Level: %d", level);
  DrawText(buffer, SCREEN_WIDTH - MeasureText(buffer, LARGE_FONT) - 10,
           45 + LARGE_FONT + 10, LARGE_FONT, WHITE);

  if (paused) {
    char *paused = "PAUSED";
    DrawText(paused, SCREEN_WIDTH - MeasureText(paused, LARGE_FONT) - 50,
             SCREEN_HEIGHT - 50, LARGE_FONT, WHITE);
  }

  if (gameOver) {
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
  updateGameState();
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROJECT_NAME);

  SetTargetFPS(60);

  initGame();

  while (!WindowShouldClose()) {
    frame();
  }

  CloseWindow();

  exit(EXIT_SUCCESS);
}
