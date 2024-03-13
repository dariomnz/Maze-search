#ifndef Maze_H
#define Maze_H

#include "raylib.h"

#define MAZE_X 100
#define MAZE_Y 100
#define SIZE_BLOCK 10
#define SIZE_BLOCK_BORDER 2

typedef enum DIRECTION
{
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_SIZE,
} DIRECTION;

typedef struct Cell
{
    int x;
    int y;
    DIRECTION neighbours[DIR_SIZE];
    bool visited;
} Cell;

typedef struct Darray_Cell
{
    Cell **items;
    int count;
    int capacity;
} Darray_Cell;

typedef struct Maze
{
    int sizeX;  // Size of x of array data
    int sizeY;  // Size of y of array data
    Cell *data; // Array of ints
} Maze;

void Maze_Generate(void);

void Maze_Init(int sizeX, int sizeY);

void Maze_Logic(void);
void Maze_Draw(void);

void Maze_Close(void);

#endif // Maze_H