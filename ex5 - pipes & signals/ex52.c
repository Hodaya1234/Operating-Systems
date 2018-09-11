//
// Hodaya Koslowsky
// 313377673
//

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#define END 'q'
#define LEFT 'a'
#define RIGHT 'd'
#define TURN 'w'
#define DOWN 's'
#define BUFFER_SIZE 1
#define BOARD_SIZE 20
#define BLOCK_SIZE 3
#define X_START_POINT 9
#define EMPTY_CHAR ' '
#define BLOCK_CHAR '-'
#define FRAME_CHAR '*'
#define BOARD_LIM 3

/* Typedef */

typedef enum {TRUE, FALSE} Bool;

typedef struct {
  int x;
  int y;
} Point;

typedef struct {
  int numOfPoints;
  int leftMost;
  int rightMost;
  int downMost;
  Point *points;
} Block;

typedef struct {
  char spots[BOARD_SIZE][BOARD_SIZE];
} Board;

/* Declarations */

Board* CreateEmptyBoard();

Block* CreateFirstBlock(int numOfPoints);

void CopyBlock(Block *dest, Block *src);

void FreeBlock(Block* block);

void FreeBoard(Board *board);

void AddBlockToBoard(Board *board, Block *block);

void ClearBlock(Board* board, Block* block);

void DrawBoard(Board *board);

void DownBlock(Board* board, Block* block);

void RightBlock(Board* board, Block* block);

void LeftBlock(Board* board, Block* block);

void FlipBlock(Board* board, Block* block);

Bool ShouldRemoveBlock(Block* block);

void SignalHandler(int sig);

void AlarmHandle(int sig);

/* Global */

int shouldStop = 0;
Board *board;
Block *block;

/* Definitions */

int main(){
  board = CreateEmptyBoard();
  DrawBoard(board);
  block = CreateFirstBlock(BLOCK_SIZE);
  AddBlockToBoard(board, block);

  /* Creating the Alarm Signal */
  struct sigaction usr_action1;
  sigset_t block_mask1;
  sigaddset(&block_mask1, SIGUSR2);
  usr_action1.sa_handler = AlarmHandle;
  usr_action1.sa_mask = block_mask1;
  usr_action1.sa_flags = 0;
  sigaction(SIGALRM, &usr_action1, NULL);

  /* Creating the SIGUSR Signal */
  struct sigaction usr_action2;
  sigset_t block_mask2;
  sigaddset(&block_mask2, SIGALRM);
  usr_action2.sa_handler = SignalHandler;
  usr_action2.sa_mask = block_mask2;
  usr_action2.sa_flags = 0;
  sigaction(SIGUSR2, &usr_action2, NULL);

  sigset_t both_signals;
  sigaddset(&both_signals, SIGUSR2);
  sigaddset(&both_signals, SIGALRM);

  sigset_t pending;

  alarm(1);
  while(shouldStop == 0){
    if (ShouldRemoveBlock(block) == TRUE){
      sigprocmask(SIG_BLOCK, &both_signals, NULL);
      ClearBlock(board, block);
      FreeBlock(block);
      block = CreateFirstBlock(BLOCK_SIZE);
      AddBlockToBoard(board, block);
      DrawBoard(board);
      sigprocmask(SIG_UNBLOCK, &both_signals, NULL);
    }
    sigpending(&pending);
    if (sigismember(&pending, SIGALRM) == 1){
      raise(SIGALRM);
    }
    if(sigismember(&pending, SIGUSR2)){
      raise(SIGUSR2);
    }
    pause();
  }
  system("clear");
  printf("Bye!\n");
  FreeBlock(block);
  FreeBoard(board);
  return 0;
}

void SignalHandler(int sig){
  if(ShouldRemoveBlock(block) == TRUE){
    return;
  }
  //signal(SIGUSR2, SignalHandler);
  if(sig == SIGUSR2){
    char buffer[BUFFER_SIZE];
    read(0, buffer, sizeof(buffer));
    char pushed = buffer[0];
    switch(pushed){
      case END:
        shouldStop = 1;
        break;
      case LEFT:
        LeftBlock(board, block);
        break;
      case RIGHT:
        RightBlock(board, block);
        break;
      case DOWN:
        DownBlock(board, block);
        break;
      case TURN:
        FlipBlock(board, block);
        break;
      default:
        break;
    }
    DrawBoard(board);
  }
}

void AlarmHandle(int sig){
  //signal(SIGALRM, AlarmHandle);
  if(sig == SIGALRM && ShouldRemoveBlock(block) == FALSE){
    DownBlock(board, block);
    DrawBoard(board);
  }
  alarm(1);
}

Board* CreateEmptyBoard(){
  Board * board = malloc(sizeof(Board));
  int i, j;
  for(i = 0; i < BOARD_SIZE - 1; i++){
    for(j = 1; j < BOARD_SIZE - 1; j++){
      board->spots[i][j] = EMPTY_CHAR;
    }
  }

  for(j = 0; j < BOARD_SIZE - 1; j++) {
    board->spots[j][0] = FRAME_CHAR;
    board->spots[j][BOARD_SIZE - 1] = FRAME_CHAR;
  }
  for(i = 0; i < BOARD_SIZE; i++){
    board->spots[BOARD_SIZE - 1][i] = FRAME_CHAR;
  }
  return board;
}

Block* CreateFirstBlock(int numOfPoints){
  Block *block = malloc(sizeof(block));
  block->numOfPoints = numOfPoints;
  block->points = malloc(sizeof(Point) * numOfPoints);
  int i;
  for (i = 0; i < numOfPoints; i++){
    block->points[i].y = 0;
    block->points[i].x = X_START_POINT + i;
  }
  block->downMost = 0;
  block->rightMost = X_START_POINT + numOfPoints - 1;
  block->leftMost = X_START_POINT;
  return block;
}

void CopyBlock(Block *dest, Block *src){
  dest->numOfPoints = src->numOfPoints;
  dest->leftMost = src->leftMost;
  dest->rightMost = src->rightMost;
  dest->downMost = src->downMost;
  for (int i = 0; i < src->numOfPoints; i++){
    dest->points[i].x = src->points[i].x;
    dest->points[i].y = src->points[i].y;
  }

}

void FreeBlock(Block* block){
  if(block == NULL || block->points == NULL){
    return;
  }
  free(block->points);
  free(block);
}

void FreeBoard(Board *board){
  if(board == NULL) {
    return;
  }
  free(board);

}

void AddBlockToBoard(Board *board, Block *block){
  int i, x, y;
  for(i = 0; i < block->numOfPoints; i++){
    x = block->points[i].x;
    y = block->points[i].y;
    board->spots[y][x] = BLOCK_CHAR;
  }
}

void ClearBlock(Board* board, Block* block){
  int i;
  for (i = 0; i < block->numOfPoints; i++){
    Point p = block->points[i];
    board->spots[p.y][p.x] = EMPTY_CHAR;
  }
}

void DrawBoard(Board *board){
  system("clear");
  printf("Tetris Game\n");
  int i,j;
  for (i = 0; i < BOARD_SIZE; i++){
    for (j = 0; j < BOARD_SIZE; j++){
      printf("%c", board->spots[i][j]);
    }
    printf("\n");
  }
  printf("%c - End Game, %c - Right, %c - Left, %c - Down, %c - Flip\n", END, RIGHT, LEFT, DOWN, TURN);
}

void DownBlock(Board* board, Block* block){
  if(block->rightMost > BOARD_SIZE - BOARD_LIM){
    return;
  }
  int i;
  for (i = 0; i < block->numOfPoints; i++){
    Point p = block->points[i];
    board->spots[p.y][p.x] = EMPTY_CHAR;
    block->points[i].y ++;
  }
  block->downMost ++;
  AddBlockToBoard(board, block);
}

void RightBlock(Board* board, Block* block){
  if(block->rightMost > BOARD_SIZE - BOARD_LIM - 1){
    return;
  }
  int i;
  for (i = 0; i < block->numOfPoints; i++){
    Point p = block->points[i];
    board->spots[p.y][p.x] = EMPTY_CHAR;
    block->points[i].x ++;
  }
  block->rightMost ++;
  block->leftMost ++;
  AddBlockToBoard(board, block);
}

void LeftBlock(Board* board, Block* block){
  if(block->leftMost < BOARD_LIM){
    return;
  }
  int i;
  for (i = 0; i < block->numOfPoints; i++){
    Point p = block->points[i];
    board->spots[p.y][p.x] = EMPTY_CHAR;
    block->points[i].x --;
  }
  block->leftMost --;
  block->rightMost --;
  AddBlockToBoard(board, block);
}

void FlipBlock(Board* board, Block* block){
  Block copy;
  copy.points = malloc(sizeof(Point) * block->numOfPoints);
  CopyBlock(&copy, block);
  Point center = copy.points[copy.numOfPoints / 2];
  int centerX = center.x;
  int centerY = center.y;
  int diff;
  int rightMost = centerX;
  int leftMost = centerX;
  int downMost = centerY;
  for(int i = 0; i < copy.numOfPoints; i++){
    Point p = copy.points[i];
    if (p.x != centerX && p.y == centerY){
      diff = centerX - p.x;
      copy.points[i].x = centerX;
      copy.points[i].y = centerY + diff;

      if (copy.points[i].x > rightMost){
        rightMost = copy.points[i].x;
      }
      if (copy.points[i].x < leftMost){
        leftMost = copy.points[i].x;
      }
      if (copy.points[i].y > downMost){
        downMost = copy.points[i].y;
      }

    } else if (p.y != centerY && p.x == centerX){
      diff = centerY - p.y;
      copy.points[i].x = centerX + diff;
      copy.points[i].y = centerY;

      if (copy.points[i].x > rightMost){
        rightMost = copy.points[i].x;
      }
      if (copy.points[i].x < leftMost){
        leftMost = copy.points[i].x;
      }
      if (copy.points[i].y > downMost){
        downMost = copy.points[i].y;
      }
    }
  }
  if (rightMost <= BOARD_SIZE - BOARD_LIM && leftMost >= BOARD_LIM && downMost < BOARD_SIZE - BOARD_LIM){
    ClearBlock(board, block);
    CopyBlock(block, &copy);
    block->rightMost = rightMost;
    block->leftMost = leftMost;
    block->downMost = downMost;
    AddBlockToBoard(board, block);
  }
  free(copy.points);
}

Bool ShouldRemoveBlock(Block* block){
  if (block->downMost > BOARD_SIZE - BOARD_LIM){
    return TRUE;
  }
  return FALSE;
}