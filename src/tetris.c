#include "tetris.h"
#include <math.h>

void initPiece(Piece *piecePtr, PieceType type) {
  piecePtr->position = (Vector3){.x = floor((double)X_SIZE / 2 - 2), .y = 0.0f};
  piecePtr->type = type;

  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      piecePtr->grid[i][j] = EMPTY;
    }
  }
  switch (piecePtr->type) {
  case J:
    piecePtr->grid[0][0] = FILLED;
    piecePtr->grid[0][1] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->grid[2][1] = FILLED;
    piecePtr->color = MY_BLUE;
    break;
  case L:
    piecePtr->grid[0][1] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->grid[2][1] = FILLED;
    piecePtr->grid[2][0] = FILLED;
    piecePtr->color = MY_ORANGE;
    break;
  case O:
    piecePtr->grid[1][0] = FILLED;
    piecePtr->grid[2][0] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->grid[2][1] = FILLED;
    piecePtr->color = MY_YELLOW;
    break;
  case T:
    piecePtr->grid[1][0] = FILLED;
    piecePtr->grid[0][1] = FILLED;
    piecePtr->grid[2][1] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->color = MY_PURPLE;
    break;
  case Z:
    piecePtr->grid[0][0] = FILLED;
    piecePtr->grid[1][0] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->grid[2][1] = FILLED;
    piecePtr->color = MY_RED;
    break;
  case S:
    piecePtr->grid[2][0] = FILLED;
    piecePtr->grid[1][0] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->grid[0][1] = FILLED;
    piecePtr->color = MY_GREEN;
    break;
  case I:
    piecePtr->grid[0][1] = FILLED;
    piecePtr->grid[1][1] = FILLED;
    piecePtr->grid[2][1] = FILLED;
    piecePtr->grid[3][1] = FILLED;
    piecePtr->color = MY_LIGHT_BLUE;
    piecePtr->position.y = -1;
    break;
  }
}

void initGame(TetrisGame *gamePtr) {
  *gamePtr = (TetrisGame){.gravitySpeed = 48,
                          .gravityCounter = 0,
                          .movementSpeed = 6,
                          .movementCounter = 0,
                          .score = 0,
                          .linesCleared = 0,
                          .level = 1,
                          .paused = false,
                          .gameOver = false};

  for (int i = 0; i < X_SIZE; ++i) {
    for (int j = 0; j < Y_SIZE; ++j) {
      gamePtr->squares[i][j].status = EMPTY;
      gamePtr->squares[i][j].color = BACKGROUND_COLOR;
    }
  }
  int val = GetRandomValue(0, TYPE_COUNT);
  initPiece(&gamePtr->piece, val);
  val = GetRandomValue(0, TYPE_COUNT);
  initPiece(&gamePtr->nextPiece, val);
}

void rotateCentered(Piece *piecePtr) {
  SquareStatus aux = piecePtr->grid[1][0];
  piecePtr->grid[1][0] = piecePtr->grid[0][1];
  piecePtr->grid[0][1] = piecePtr->grid[1][2];
  piecePtr->grid[1][2] = piecePtr->grid[2][1];
  piecePtr->grid[2][1] = aux;

  aux = piecePtr->grid[0][0];
  piecePtr->grid[0][0] = piecePtr->grid[0][2];
  piecePtr->grid[0][2] = piecePtr->grid[2][2];
  piecePtr->grid[2][2] = piecePtr->grid[2][0];
  piecePtr->grid[2][0] = aux;
}

void rotateI(Piece *piecePtr) {
  SquareStatus aux = piecePtr->grid[0][1];
  piecePtr->grid[0][1] = piecePtr->grid[1][3];
  piecePtr->grid[1][3] = piecePtr->grid[3][2];
  piecePtr->grid[3][2] = piecePtr->grid[2][0];
  piecePtr->grid[2][0] = aux;

  aux = piecePtr->grid[3][1];
  piecePtr->grid[3][1] = piecePtr->grid[1][0];
  piecePtr->grid[1][0] = piecePtr->grid[0][2];
  piecePtr->grid[0][2] = piecePtr->grid[2][3];
  piecePtr->grid[2][3] = aux;

  aux = piecePtr->grid[1][1];
  piecePtr->grid[1][1] = piecePtr->grid[1][2];
  piecePtr->grid[1][2] = piecePtr->grid[2][2];
  piecePtr->grid[2][2] = piecePtr->grid[2][1];
  piecePtr->grid[2][1] = aux;
}

void rotate(Piece *piecePtr) {
  switch (piecePtr->type) {
  case T:
  case L:
  case J:
  case S:
  case Z:
    rotateCentered(piecePtr);
    break;
  case I:
    rotateI(piecePtr);
    break;
  case O:
    break;
  }
}

bool shouldStop(Piece *piece, Square squares[X_SIZE][Y_SIZE]) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if ((piece->grid[i][j] == FILLED &&
           squares[(int)(piece->position.x + i)][(int)(piece->position.y + j)]
                   .status == FILLED) ||
          (piece->grid[i][j] == FILLED && (piece->position.y + j) == Y_SIZE)) {
        return true;
      }
    }
  }
  return false;
}

void resolveLevel(TetrisGame *gamePtr) {
  if (gamePtr->linesCleared > 10) {
    gamePtr->linesCleared = gamePtr->linesCleared - 10;
    gamePtr->level += 1;
    if (gamePtr->gravitySpeed > 6) {
      gamePtr->gravitySpeed -= 5;
    }
  }
}

int calculateScore(int numLinesCleared, int currentLevel) {
  switch (numLinesCleared) {
  case 1:
    return 40 * currentLevel;
  case 2:
    return 100 * currentLevel;
  case 3:
    return 300 * currentLevel;
  case 4:
    return 1200 * currentLevel;
  default:
    return 0;
  }
}

int clearLines(Square squares[X_SIZE][Y_SIZE]) {
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
      for (int k = j - 1; k > 0; --k) {
        for (int i = 0; i < X_SIZE; ++i) {
          squares[i][k + 1].status = squares[i][k].status;
        }
      }
    }
  }
  return numLinesCleared;
}

void fillStatuses(Piece *piece, Square squares[X_SIZE][Y_SIZE]) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED) {
        squares[(int)(piece->position.x + i)][(int)(piece->position.y + j - 1)]
            .status = FILLED;
        squares[(int)(piece->position.x + i)][(int)(piece->position.y + j - 1)]
            .color = piece->color;
      }
    }
  }
}

bool isGameOver(Piece *piece) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED && piece->position.y + j == 0) {
        return true;
      }
    }
  }
  return false;
}

bool canGoRight(Piece *piece, Square squares[X_SIZE][Y_SIZE]) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED &&
          (piece->position.x + i == X_SIZE - 1 ||
           squares[(int)(piece->position.x + i + 1)]
                  [(int)(piece->position.y + j)]
                      .status == FILLED)) {
        return false;
      }
    }
  }
  return true;
}

bool canGoLeft(Piece *piece, Square squares[X_SIZE][Y_SIZE]) {
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED &&
          (piece->position.x + i == 0 ||
           squares[(int)(piece->position.x + i - 1)]
                  [(int)(piece->position.y + j)]
                      .status == FILLED)) {
        return false;
      }
    }
  }
  return true;
}

bool canRotate(Piece *piece, Square squares[X_SIZE][Y_SIZE]) {
  // TODO FIX ME TO NOT BE IMPERATIVE
  rotate(piece);
  for (int i = 0; i < TETRI_SIZE; ++i) {
    for (int j = 0; j < TETRI_SIZE; ++j) {
      if (piece->grid[i][j] == FILLED &&
          ((piece->position.x + i < 0) || piece->position.x + i >= X_SIZE ||
           piece->position.y + j < 0 || piece->position.y + j >= Y_SIZE ||
           squares[(int)(piece->position.x + i)][(int)(piece->position.y + j)]
                   .status == FILLED)) {
        rotate(piece);
        rotate(piece);
        rotate(piece); // rotate back to previous point LOL
        return false;
      }
    }
  }
  rotate(piece);
  rotate(piece);
  rotate(piece);
  return true;
}

void updateGameState(TetrisGame *gamePtr) {
  if (!gamePtr->paused && !gamePtr->gameOver) {
    if ((IsKeyPressed(KEY_RIGHT) ||
         (IsKeyDown(KEY_RIGHT) &&
          gamePtr->movementCounter > gamePtr->movementSpeed)) &&
        canGoRight(&gamePtr->piece, gamePtr->squares)) {
      gamePtr->piece.position.x += 1;
      gamePtr->movementCounter = 0;
    }
    if ((IsKeyPressed(KEY_LEFT) ||
         (IsKeyDown(KEY_LEFT) &&
          gamePtr->movementCounter > gamePtr->movementSpeed)) &&
        canGoLeft(&gamePtr->piece, gamePtr->squares)) {
      gamePtr->piece.position.x -= 1;
      gamePtr->movementCounter = 0;
    }
    if (IsKeyDown(KEY_DOWN)) {
      gamePtr->gravityCounter += gamePtr->gravitySpeed / 2;
    } else if (IsKeyPressed(KEY_SPACE)) {
      while (!shouldStop(&gamePtr->piece, gamePtr->squares)) {
        gamePtr->piece.position.y += 1;
      }
    }

    if (IsKeyPressed(KEY_UP) && canRotate(&gamePtr->piece, gamePtr->squares)) {
      rotate(&gamePtr->piece);
    }

    if (gamePtr->movementCounter > gamePtr->movementSpeed) {
      gamePtr->movementCounter = 0;
    } else {
      ++gamePtr->movementCounter;
    }

    if (shouldStop(&gamePtr->piece, gamePtr->squares)) {
      fillStatuses(&gamePtr->piece, gamePtr->squares);
      int numLinesCleared = clearLines(gamePtr->squares);
      gamePtr->linesCleared += numLinesCleared;
      gamePtr->score += calculateScore(numLinesCleared, gamePtr->level);
      resolveLevel(gamePtr);

      gamePtr->gameOver = isGameOver(&gamePtr->piece);
      if (!gamePtr->gameOver) {
        gamePtr->piece = gamePtr->nextPiece;
        int val = GetRandomValue(0, TYPE_COUNT);
        initPiece(&gamePtr->nextPiece, val);
      }
    }
    gamePtr->gravityCounter += 1;

    if (gamePtr->gravityCounter > gamePtr->gravitySpeed) {
      gamePtr->gravityCounter = 0;
      gamePtr->piece.position.y += 1;
    }
  }
  if (IsKeyPressed(KEY_P)) {
    gamePtr->paused = !gamePtr->paused;
  }
  if (gamePtr->gameOver && IsKeyPressed(KEY_SPACE)) {
    initGame(gamePtr);
  }
}
