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

#define SCREEN_WIDTH X_SIZE *SQUARE_WIDTH
#define SCREEN_HEIGHT Y_SIZE *SQUARE_WIDTH
#define TETRI_SIZE 4
#define TYPE_COUNT 6

typedef enum SquareStatus { EMPTY, FILLED } SquareStatus;

typedef struct Square {
  SquareStatus status;
  Color color;
} Square;

static Square squares[X_SIZE][Y_SIZE];

typedef enum TetriType { I, T, O, S, Z, L, J } TetriType;

typedef struct Tetri {
  int x;
  int y;
  TetriType type;
  SquareStatus grid[TETRI_SIZE][TETRI_SIZE];
  Color color;
} Tetri;

static Tetri tetriPos;
static int gravitySpeed = 30;
static int gravityCounter = 0;
static int score = 0;

static bool paused = false;

void initTetri(TetriType type) {
  tetriPos.x = 0;
  tetriPos.y = 0;
  tetriPos.type = type;

  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      tetriPos.grid[i][j] = EMPTY;
    }
  }
  switch (tetriPos.type) {
  case J:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[2][1] = FILLED;
    tetriPos.color = BLUE;
    break;
  case L:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[0][1] = FILLED;
    tetriPos.color = ORANGE;
    break;
  case O:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[0][1] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    tetriPos.color = YELLOW;
    break;
  case T:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    tetriPos.color = PURPLE;
    break;
  case Z:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    tetriPos.grid[2][1] = FILLED;
    tetriPos.color = RED;
    break;
  case S:
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    tetriPos.grid[0][1] = FILLED;
    tetriPos.color = GREEN;
    break;
  case I:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[3][0] = FILLED;
    tetriPos.color = SKYBLUE;
    break;
  }
}

void initGame(void) {
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      squares[i][j].status = EMPTY;
      squares[i][j].color = BLACK;
    }
  }
}

void rotate(void) {
  // TODO FIX ROTATION
  SquareStatus aux = tetriPos.grid[0][0];
  tetriPos.grid[0][0] = tetriPos.grid[3][0];
  tetriPos.grid[3][0] = tetriPos.grid[3][3];
  tetriPos.grid[3][3] = tetriPos.grid[0][3];
  tetriPos.grid[0][3] = aux;

  aux = tetriPos.grid[1][0];
  tetriPos.grid[1][0] = tetriPos.grid[3][1];
  tetriPos.grid[3][1] = tetriPos.grid[2][3];
  tetriPos.grid[2][3] = tetriPos.grid[0][2];
  tetriPos.grid[0][2] = aux;

  aux = tetriPos.grid[2][0];
  tetriPos.grid[2][0] = tetriPos.grid[3][2];
  tetriPos.grid[3][2] = tetriPos.grid[1][3];
  tetriPos.grid[1][3] = tetriPos.grid[0][1];
  tetriPos.grid[0][1] = aux;

  aux = tetriPos.grid[1][1];
  tetriPos.grid[1][1] = tetriPos.grid[2][1];
  tetriPos.grid[2][1] = tetriPos.grid[2][2];
  tetriPos.grid[2][2] = tetriPos.grid[1][2];
  tetriPos.grid[1][2] = aux;
}

bool shouldStop(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if ((tetriPos.grid[i][j] == FILLED &&
           squares[tetriPos.x + i][tetriPos.y + j].status == FILLED) ||
          (tetriPos.grid[i][j] == FILLED && (tetriPos.y + j) == Y_SIZE)) {
        return true;
      }
    }
  }
  return false;
}

void clearLines(void) {
  for (int j = 0; j < Y_SIZE; ++j) {
    bool clear = true;
    for (int i = 0; i < X_SIZE; ++i) {
      if (squares[i][j].status == EMPTY) {
        clear = false;
      }
    }
    if (clear) {
      score += 1;
      for (int k = j - 1; k > 0; --k) {
        for (int i = 0; i < X_SIZE; ++i) {
          squares[i][k + 1].status = squares[i][k].status;
        }
      }
    }
  }
}

void fillStatuses(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (tetriPos.grid[i][j] == FILLED) {
        squares[tetriPos.x + i][tetriPos.y + j - 1].status = FILLED;
        squares[tetriPos.x + i][tetriPos.y + j - 1].color = tetriPos.color;
      }
    }
  }
}

void updateGameState(void) {
  if (!paused) {
    if (tetriPos.x < (X_SIZE - 1) && IsKeyPressed(KEY_RIGHT)) {
      tetriPos.x += 1;
    } else if (tetriPos.x > 0 && IsKeyPressed(KEY_LEFT)) {
      tetriPos.x -= 1;
    } else if (IsKeyDown(KEY_DOWN)) {
      gravityCounter += gravitySpeed / 5;
    } else if (IsKeyPressed(KEY_UP)) {
      rotate();
    } else if (IsKeyPressed(KEY_SPACE)) {
      while (!shouldStop()) {
        tetriPos.y += 1;
      }
    }

    gravityCounter += 1;
    if (gravityCounter > gravitySpeed) {
      gravityCounter = 0;
      tetriPos.y += 1;
    }

    if (shouldStop()) {
      fillStatuses();
      int val = GetRandomValue(0, TYPE_COUNT);
      initTetri(val);
      clearLines();
    }
  }
  if (IsKeyPressed(KEY_P)) {
    paused = !paused;
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

void drawTetri(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (tetriPos.grid[i][j] == FILLED) {
        switch (tetriPos.type) {
        default:
          drawSquare(tetriPos.x + i, tetriPos.y + j, tetriPos.color);
          break;
        }
      }
    }
  }
}

void drawGame(void) {
  drawTetri();
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      if (squares[i][j].status == FILLED) {
        drawSquare(i, j, squares[i][j].color);
      }
    }
  }
}

void frame(void) {
  BeginDrawing();
  ClearBackground(BLACK);
  drawGame();
  DrawFPS(SCREEN_WIDTH - 80, 10);
  char buffer[40];
  sprintf(buffer, "Score: %d", score);
  DrawText(buffer, SCREEN_WIDTH - (strlen(buffer) * 12), 45, 20, WHITE);

  if (paused) {
    DrawText("PAUSED", SCREEN_WIDTH / 2 - 6 * 6, SCREEN_HEIGHT / 2, 20, WHITE);
  }
  EndDrawing();
  updateGameState();
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROJECT_NAME);

  SetTargetFPS(60);

  initGame();
  int val = GetRandomValue(0, TYPE_COUNT);
  initTetri(val);

  while (!WindowShouldClose()) {
    frame();
  }

  CloseWindow();

  exit(EXIT_SUCCESS);
}
