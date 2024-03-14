#include "Maze.h"
#include "DArray.h"
#include "Debug.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>

Maze maze = {0};
Darray_Cell stack = {0};
float MAZE_ASYNC_VEL = 0.1f;

int Maze_GetMAZE_ASYNC_VEL()
{
    return MAZE_ASYNC_VEL;
}

void Maze_SetMAZE_ASYNC_VEL(float value)
{
    MAZE_ASYNC_VEL = value;
}

Cell *Maze_GetCell(int x, int y)
{
    if (x < 0 || x >= maze.sizeX)
        DEBUG_ERROR("Maze x (%d) out of limits (%d)", x, maze.sizeX);
    if (y < 0 || y >= maze.sizeY)
        DEBUG_ERROR("Maze y (%d) out of limits (%d)", y, maze.sizeY);
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
            cell->red = 0;
            for (DIRECTION i = 0; i < DIR_SIZE; i++)
            {
                cell->neighbours[i] = false;
            }
        }
    }
}

bool is_generate = false;
clock_t t;
void Maze_Generate_async(void)
{
    if (!is_generate)
    {
        return;
    }

    Cell *cell, *chosen_cell;
    DIRECTION dir;
    for (int i = 0; i < MAZE_ASYNC_VEL * maze.sizeX * maze.sizeY && stack.count > 0; i++)
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
            chosen_cell->red = 255;
        }
    }
    if (stack.count <= 0)
    {
        is_generate = false;

        t = clock() - t;
        double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
        DEBUG_MSG("Maze_Generate() took %f seconds to execute", time_taken);
    }
}
void Maze_Generate(bool async)
{
    DEBUG_MSG("Maze_Generate() started");

    // Reset
    Maze_Reset();

    Cell *cell, *chosen_cell;
    DIRECTION dir;
    int x = rand() % maze.sizeX;
    int y = rand() % maze.sizeY;
    cell = Maze_GetCell(x, y);
    cell->visited = true;
    DArray_clear(&stack);
    DArray_append(&stack, cell);
    t = clock();
    if (!async)
    {
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
                chosen_cell->red = 255;
            }
        }
        t = clock() - t;
        double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
        DEBUG_MSG("Maze_Generate() took %f seconds to execute", time_taken);
    }
    else
    {
        is_generate = true;
    }
}

void Maze_Init(int sizeX, int sizeY)
{
    int data_size = sizeX * sizeY * sizeof(Cell);
    maze.sizeX = sizeX;
    maze.sizeY = sizeY;
    maze.data = realloc(maze.data, data_size);
    if (maze.data == NULL)
    {
        DEBUG_ERROR("realloc of size: %d", data_size);
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
    Maze_Generate_async();
}

void Maze_Draw(void)
{
    // Draw cell background
    Cell *cell;
    DrawRectangle(0, 0, maze.sizeX * SIZE_BLOCK, maze.sizeY * SIZE_BLOCK, WHITE);
    for (int x = 0; x < maze.sizeX; x++)
    {
        for (int y = 0; y < maze.sizeY; y++)
        {
            cell = Maze_GetCell(x, y);

            if (cell->red > MAZE_ASYNC_VEL)
            {
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, (Color){255, 0, 0, cell->red});
                cell->red -= MAZE_ASYNC_VEL;
            }
            // Draw cell borders
            if (cell->neighbours[DIR_UP] == false)
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK + SIZE_BLOCK_BORDER, SIZE_BLOCK_BORDER, BLACK);

            if (cell->neighbours[DIR_LEFT] == false)
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK_BORDER, SIZE_BLOCK + SIZE_BLOCK_BORDER, BLACK);
        }
    }
}

void Maze_Close(void)
{
    free(maze.data);
    maze.sizeX = 0;
    maze.sizeY = 0;
    DArray_free(&stack);
}