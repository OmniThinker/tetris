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
static SquareStatus statuses[X_SIZE][Y_SIZE];

typedef enum TetriType { I, T, O, S, Z, L, J } TetriType;

typedef struct Tetri {
  int x;
  int y;
  TetriType type;
  SquareStatus grid[TETRI_SIZE][TETRI_SIZE];
} Tetri;

static Tetri tetriPos;
static int gravitySpeed = 30;
static int gravityCounter = 0;
static int score = 0;

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
    break;
  case L:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[0][1] = FILLED;
    break;
  case O:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[0][1] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    break;
  case T:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    break;
  case Z:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    tetriPos.grid[2][1] = FILLED;
    break;
  case S:
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[1][1] = FILLED;
    tetriPos.grid[0][1] = FILLED;
    break;
  case I:
    tetriPos.grid[0][0] = FILLED;
    tetriPos.grid[1][0] = FILLED;
    tetriPos.grid[2][0] = FILLED;
    tetriPos.grid[3][0] = FILLED;
    break;
  }
}

void initGame(void) {
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      statuses[i][j] = EMPTY;
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
           statuses[tetriPos.x + i][tetriPos.y + j] == FILLED) ||
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
      if (statuses[i][j] == EMPTY) {
        clear = false;
      }
    }
    if (clear) {
      score += 1;
      for (int k = j - 1; k > 0; --k) {
        for (int i = 0; i < X_SIZE; ++i) {
          statuses[i][k + 1] = statuses[i][k];
        }
      }
    }
  }
}

void fillStatuses(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (tetriPos.grid[i][j] == FILLED) {
        statuses[tetriPos.x + i][tetriPos.y + j - 1] = FILLED;
      }
    }
  }
}

void updateGameState(void) {

  if (tetriPos.x < (X_SIZE - 1) && IsKeyPressed(KEY_RIGHT)) {
    tetriPos.x += 1;
  } else if (tetriPos.x > 0 && IsKeyPressed(KEY_LEFT)) {
    tetriPos.x -= 1;
  } else if (IsKeyDown(KEY_DOWN)) {
    gravityCounter += gravitySpeed / 5;
  } else if (IsKeyPressed(KEY_UP)) {
    rotate();
  }

  if (shouldStop()) {
    fillStatuses();
    int val = GetRandomValue(0, TYPE_COUNT);
    initTetri(val);
    clearLines();
  }

  gravityCounter += 1;
  if (gravityCounter > gravitySpeed) {
    gravityCounter = 0;
    tetriPos.y += 1;
  }
}

// IMPERATIVE

void printStatus(void) {
  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      if (statuses[i][j] == FILLED) {
        printf("X");
      } else {
        printf("_");
      }
    }
    printf("\n");
  }
}
void drawSquare(int x, int y, Color color) {
  // X needs to be between 10 and 0, Y needs to be between 0 and 22
  DrawRectangle(x * SQUARE_WIDTH, y * SQUARE_WIDTH, SQUARE_WIDTH, SQUARE_WIDTH,
                color);
}

void drawTetri(void) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (tetriPos.grid[i][j] == FILLED) {
        switch (tetriPos.type) {
        default:
          drawSquare(tetriPos.x + i, tetriPos.y + j, BLUE);
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
      if (statuses[i][j] == FILLED) {
        drawSquare(i, j, WHITE);
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
