#include "Maze.h"
#include "DArray.h"
#include "Debug.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>

Maze maze = {0};

Cell *Maze_GetCell(int x, int y)
{
    return &maze.data[x * maze.sizeY + y];
}

bool Maze_AnyNeighbour(int x, int y)
{
    Cell *cell = Maze_GetCell(x, y);
    bool output;
    for (DIRECTION i = 0; i < DIR_SIZE; i++)
    {
        output |= cell->neighbours[i];
    }
    return output;
}

Cell *Maze_GetNeighbour(int x, int y, DIRECTION dir)
{
    switch (dir)
    {
    case DIR_UP:
        if (y == 0)
            return NULL;
        return Maze_GetCell(x, y - 1);
    case DIR_DOWN:
        if (y == maze.sizeY - 1)
            return NULL;
        return Maze_GetCell(x, y + 1);
    case DIR_LEFT:
        if (x == 0)
            return NULL;
        return Maze_GetCell(x - 1, y);
    case DIR_RIGHT:
        if (x == maze.sizeX - 1)
            return NULL;
        return Maze_GetCell(x + 1, y);
    default:
        DEBUG_ERROR("incorrect dir %d", dir);
        return NULL;
    }
}

DIRECTION Maze_GetRandUnvisitedNeighbour(int x, int y)
{
    Cell *cell_neig;
    int repetition = 0;
    for (DIRECTION i = rand() % DIR_SIZE; repetition < DIR_SIZE; i = (i + 1) % DIR_SIZE)
    {
        cell_neig = Maze_GetNeighbour(x, y, i);
        if (cell_neig == NULL)
            continue;
        if (!cell_neig->visited)
            return i;
        repetition++;
    }
    return DIR_SIZE;
}

DIRECTION Maze_OpositeDir(DIRECTION dir)
{
    switch (dir)
    {
    case DIR_UP:
        return DIR_DOWN;
    case DIR_DOWN:
        return DIR_UP;
    case DIR_LEFT:
        return DIR_RIGHT;
    case DIR_RIGHT:
        return DIR_LEFT;
    default:
        DEBUG_ERROR("incorrect dir %d", dir);
        return DIR_SIZE;
    }
}

void Maze_SetNeighbor(int x, int y, DIRECTION dir, bool value)
{
    Cell *cell = NULL, *cell_neig = NULL;
    DIRECTION dir_neig = Maze_OpositeDir(dir);

    cell = Maze_GetCell(x, y);
    cell_neig = Maze_GetNeighbour(x, y, dir);
    if (cell_neig == NULL)
        return;
    cell->neighbours[dir] = value;
    cell_neig->neighbours[dir_neig] = value;
}

void Maze_Reset(void)
{
    Cell *cell;
    for (int x = 0; x < maze.sizeX; x++)
    {
        for (int y = 0; y < maze.sizeY; y++)
        {
            cell = Maze_GetCell(x, y);
            cell->visited = false;
            for (DIRECTION i = 0; i < DIR_SIZE; i++)
            {
                cell->neighbours[i] = false;
            }
        }
    }
}

void Maze_Generate(void)
{
    DEBUG_MSG("Maze_Generate() started");
    clock_t t;
    t = clock();
    // Reset
    Maze_Reset();

    Darray_Cell stack = {0};
    Cell *cell, *chosen_cell;
    DIRECTION dir;
    int x = rand() % maze.sizeX;
    int y = rand() % maze.sizeY;
    cell = Maze_GetCell(x, y);
    cell->visited = true;
    DArray_append(&stack, cell);
    while (stack.count > 0)
    {
        // Pop cell
        cell = stack.items[stack.count - 1];
        DArray_remove(&stack, stack.count - 1);

        // Check neighbours
        dir = Maze_GetRandUnvisitedNeighbour(cell->x, cell->y);
        if (dir != DIR_SIZE)
        {
            // Push cell
            DArray_append(&stack, cell);
            // Choose one
            Maze_SetNeighbor(cell->x, cell->y, dir, true);
            chosen_cell = Maze_GetNeighbour(cell->x, cell->y, dir);
            chosen_cell->visited = true;
            DArray_append(&stack, chosen_cell);
        }
    }
    DArray_free(&stack);

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    DEBUG_MSG("Maze_Generate() took %f seconds to execute", time_taken);
}

void Maze_Init(int sizeX, int sizeY)
{
    int data_size = sizeX * sizeY * sizeof(Cell);
    maze.sizeX = sizeX;
    maze.sizeY = sizeY;
    maze.data = malloc(data_size);
    if (maze.data == NULL)
    {
        DEBUG_ERROR("Malloc of size: %d", data_size);
        exit(1);
    }
    // Set neighbours to false
    memset(maze.data, 0, data_size);

    // Set x and y
    Cell *cell;
    for (int x = 0; x < sizeX; x++)
    {
        for (int y = 0; y < sizeY; y++)
        {
            cell = Maze_GetCell(x, y);
            cell->x = x;
            cell->y = y;
        }
    }
}

void Maze_Logic(void)
{
    // if (IsKeyPressed(KEY_SPACE))
    // {
    //     DEBUG_MSG("Maze_Generate() start");
    //     clock_t t;
    //     t = clock();

    //     Maze_Generate();

    //     t = clock() - t;
    //     double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    //     DEBUG_MSG("Maze_Generate() took %f seconds to execute \n", time_taken);
    // }
}

void Maze_Draw(void)
{
    for (int x = 0; x < maze.sizeX; x++)
    {
        for (int y = 0; y < maze.sizeY; y++)
        {
            // Draw cell background
            DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, WHITE);
            // Draw cell borders
            if (Maze_GetCell(x, y)->neighbours[DIR_UP] == false)
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK_BORDER, BLACK);

            if (Maze_GetCell(x, y)->neighbours[DIR_LEFT] == false)
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK_BORDER, SIZE_BLOCK, BLACK);

            // if (Maze_GetCell(x, y)->neighbours[DIR_DOWN] == false)
            //     DrawRectangle(x * SIZE_BLOCK, ((y + 1) * SIZE_BLOCK) - SIZE_BLOCK_BORDER, SIZE_BLOCK, SIZE_BLOCK_BORDER, BLACK);

            // if (Maze_GetCell(x, y)->neighbours[DIR_RIGHT] == false)
            //     DrawRectangle(((x + 1) * SIZE_BLOCK) - SIZE_BLOCK_BORDER, y * SIZE_BLOCK, SIZE_BLOCK_BORDER, SIZE_BLOCK, BLACK);
        }
    }
}

void Maze_Close(void)
{
    free(maze.data);
    maze.sizeX = 0;
    maze.sizeY = 0;
}