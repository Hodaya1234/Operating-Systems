//
// Created by h on 15/06/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BOARD_SIZE 20
#define X_START_POINT 9
#define EMPTY_CHAR ' '
#define BLOCK_CHAR '-'
#define BOARD_LIM 3

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

Board* CreateEmptyBoard(){
  Board * board = malloc(sizeof(Board));
  int i, j;
  for(i = 0; i < BOARD_SIZE - 1; i++){
    for(j = 1; j < BOARD_SIZE - 1; j++){
      board->spots[i][j] = EMPTY_CHAR;
    }
  }

  for(j = 0; j < BOARD_SIZE - 1; j++) {
    board->spots[j][0] = '*';
    board->spots[j][BOARD_SIZE - 1] = '*';
  }
  for(i = 0; i < BOARD_SIZE; i++){
    board->spots[BOARD_SIZE - 1][i] = '*';
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
  int i,j;
  for (i = 0; i < BOARD_SIZE; i++){
    for (j = 0; j < BOARD_SIZE; j++){
      printf("%c", board->spots[i][j]);
    }
    printf("\n");
  }
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
  if(block->rightMost > BOARD_SIZE - BOARD_LIM){
    return;
  }
  int i;
  for (i = 0; i < block->numOfPoints; i++){
    Point p = block->points[i];
    board->spots[p.y][p.x] = EMPTY_CHAR;
    block->points[i].x ++;
  }
  block->rightMost ++;
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
