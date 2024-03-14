#ifndef Maze_H
#define Maze_H

#include "raylib.h"

#define MAZE_WIDTH 20
#define MAZE_HEIGHT 20
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

typedef enum CELL_TYPE
{
    TYPE_NULL = 0,
    TYPE_START,
    TYPE_END,
    TYPE_PATH,
    TYPE_SIZE,
} CELL_TYPE;

typedef enum SEARCH_TYPE
{
    SEARCH_NULL = 0,
    SEARCH_AMPLITUDE,
    SEARCH_DEPTH,
    SEARCH_TYPE_SIZE,
} SEARCH_TYPE;

typedef struct Cell
{
    int x;
    int y;
    DIRECTION neighbours[DIR_SIZE];
    bool visited;
    unsigned char heat;
    CELL_TYPE type;
    DIRECTION path_dir;
} Cell;

typedef struct Darray_Cell
{
    Cell **items;
    int count;
    int capacity;
} Darray_Cell;

typedef struct Maze
{
    int sizeX;        // Size of x of array data
    int sizeY;        // Size of y of array data
    Cell *data;       // Array of ints
    Cell *start_cell; // Pointer to start cell
    Cell *end_cell;   // Pointer to end cell
} Maze;

int Maze_GetMAZE_ASYNC_VEL();
void Maze_SetMAZE_ASYNC_VEL(float value);

void Maze_Generate(bool async);
void Maze_Resolve(SEARCH_TYPE type);

void Maze_Init(int sizeX, int sizeY);

void Maze_Logic(void);
void Maze_Draw(void);

void Maze_Close(void);

#endif // Maze_H