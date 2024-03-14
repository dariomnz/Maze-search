#include "Maze.h"
#include "DArray.h"
#include "Debug.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>
#include "raymath.h"
#include "GUI.h"

Maze maze = {0};
Darray_Cell stack = {0};
float MAZE_ASYNC_VEL = 0.001f;
Camera2D camera = {0};
Vector2 prevMousePos = {0};
bool is_generate = false;
SEARCH_TYPE searching_type = SEARCH_NULL;
clock_t t;

int Maze_GetMAZE_ASYNC_VEL()
{
    return MAZE_ASYNC_VEL;
}

void Maze_SetMAZE_ASYNC_VEL(float value)
{
    MAZE_ASYNC_VEL = value;
}

bool Maze_EqualCell(Cell *cell1, Cell *cell2)
{
    if (cell1 == NULL || cell2 == NULL)
        return false;
    return cell1->x == cell2->x && cell1->y == cell2->y;
}

Cell *Maze_GetCell(int x, int y)
{
    if (x < 0 || x >= maze.sizeX)
        DEBUG_ERROR("Maze x (%d) out of limits (%d)", x, maze.sizeX);
    if (y < 0 || y >= maze.sizeY)
        DEBUG_ERROR("Maze y (%d) out of limits (%d)", y, maze.sizeY);
    return &maze.data[x * maze.sizeY + y];
}

Cell *Maze_GetCellWorld(int x, int y)
{
    if (x < 0 || x >= maze.sizeX * SIZE_BLOCK)
    {
        // DEBUG_ERROR("Maze x world (%d) out of limits (%d)", x, maze.sizeX * SIZE_BLOCK);
        return NULL;
    }
    if (y < 0 || y >= maze.sizeY * SIZE_BLOCK)
    {
        // DEBUG_ERROR("Maze y world (%d) out of limits (%d)", y, maze.sizeY * SIZE_BLOCK);
        return NULL;
    }
    return &maze.data[x / SIZE_BLOCK * maze.sizeY + y / SIZE_BLOCK];
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

DIRECTION Maze_GetRandUnvisitedPosibleNeighbour(int x, int y)
{
    Cell *cell_neig;
    int repetition = 0;
    for (DIRECTION i = rand() % DIR_SIZE; repetition < DIR_SIZE; i = (i + 1) % DIR_SIZE)
    {
        cell_neig = Maze_GetNeighbour(x, y, i);
        if (cell_neig == NULL || Maze_GetCell(x, y)->neighbours[i] == false)
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

void Maze_Reset(bool reset_neighbours)
{
    Cell *cell;
    for (int x = 0; x < maze.sizeX; x++)
    {
        for (int y = 0; y < maze.sizeY; y++)
        {
            cell = Maze_GetCell(x, y);
            cell->visited = false;
            cell->type = TYPE_NULL;
            cell->heat = 0;
            if (reset_neighbours)
                for (DIRECTION i = 0; i < DIR_SIZE; i++)
                {
                    cell->neighbours[i] = false;
                }
        }
    }
}

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
            chosen_cell->heat = 255;
        }
    }
    if (stack.count <= 0)
    {
        is_generate = false;

        for (int i = 0; i < maze.sizeX * maze.sizeY * 0.1f; i++)
        {
            int x = rand() % maze.sizeX;
            int y = rand() % maze.sizeY;
            cell = Maze_GetCell(x, y);
            dir = rand() % DIR_SIZE;
            chosen_cell = Maze_GetNeighbour(cell->x, cell->y, dir);
            if (chosen_cell)
            {
                Maze_SetNeighbor(x, y, dir, true);
                cell->heat = 255;
            }
        }

        double time_taken = ((double)(clock() - t)) / CLOCKS_PER_SEC; // in seconds
        DEBUG_MSG("Maze_Generate() took %f seconds to execute", time_taken);
    }
}
void Maze_Generate(bool async)
{
    DEBUG_MSG("Maze_Generate() started");

    // Reset
    Maze_Reset(true);

    Cell *cell;
    int x = rand() % maze.sizeX;
    int y = rand() % maze.sizeY;
    cell = Maze_GetCell(x, y);
    cell->visited = true;
    DArray_clear(&stack);
    DArray_append(&stack, cell);
    t = clock();
    is_generate = true;
    if (!async)
    {
        while (stack.count > 0)
        {
            Maze_Generate_async();
        }
    }
}

void Maze_SelectStartEnd(Cell *cell)
{
    if (cell == NULL)
        return;

    if (maze.start_cell == NULL)
    {
        maze.start_cell = cell;
        cell->type = TYPE_START;
    }
    else if (maze.end_cell == NULL)
    {
        maze.end_cell = cell;
        cell->type = TYPE_END;
    }
    else if (maze.start_cell != NULL)
    {
        maze.start_cell->type = TYPE_NULL;

        maze.start_cell = cell;
        cell->type = TYPE_START;
        maze.end_cell->type = TYPE_NULL;
        maze.end_cell = NULL;
    }
}

void Maze_Resolve_Depth_Async(void)
{
    DIRECTION dir;
    bool find = false;
    Cell *cell, *chosen_cell;
    // Pop cell
    cell = stack.items[stack.count - 1];
    DArray_remove(&stack, stack.count - 1);

    dir = Maze_GetRandUnvisitedPosibleNeighbour(cell->x, cell->y);
    if (dir != DIR_SIZE)
    {
        // Push cell
        DArray_append(&stack, cell);
        // Choose one
        chosen_cell = Maze_GetNeighbour(cell->x, cell->y, dir);
        chosen_cell->visited = true;
        DArray_append(&stack, chosen_cell);
        chosen_cell->heat = 255;
        if (Maze_EqualCell(chosen_cell, maze.end_cell))
        {
            find = true;
        }
    }

    if (stack.count <= 0 || find == true)
    {
        searching_type = SEARCH_NULL;
        // Mark path
        for (int i = 0; i < stack.count; i++)
        {
            if (Maze_EqualCell(stack.items[i], maze.start_cell))
                stack.items[i]->type = TYPE_START;
            else if (Maze_EqualCell(stack.items[i], maze.end_cell))
                stack.items[i]->type = TYPE_END;
            else
                stack.items[i]->type = TYPE_PATH;
        }
    }
}

void Maze_Resolve_Amplitude_Async(void)
{
    // Pop cell
    bool find = false;
    Cell *cell, *chosen_cell;
    cell = stack.items[0];
    DArray_remove(&stack, 0);

    for (int i = 0; i < DIR_SIZE; i++)
    {
        chosen_cell = Maze_GetNeighbour(cell->x, cell->y, i);
        if (chosen_cell == NULL || Maze_GetCell(cell->x, cell->y)->neighbours[i] == false)
            continue;
        if (!chosen_cell->visited)
        {
            chosen_cell->path_dir = Maze_OpositeDir(i);
            chosen_cell->visited = true;
            DArray_append(&stack, chosen_cell);
            chosen_cell->heat = 255;
            if (Maze_EqualCell(chosen_cell, maze.end_cell))
            {
                find = true;
            }
        }
    }
    if (stack.count <= 0 || find == true)
    {
        searching_type = SEARCH_NULL;
        // Mark path
        cell = maze.end_cell;
        int repetitions = 0;
        while (repetitions < maze.sizeX * maze.sizeY)
        {
            if (Maze_EqualCell(cell, maze.end_cell))
                cell->type = TYPE_END;
            else
                cell->type = TYPE_PATH;

            chosen_cell = Maze_GetNeighbour(cell->x, cell->y, cell->path_dir);
            if (chosen_cell == NULL)
                break;

            if (Maze_EqualCell(chosen_cell, maze.start_cell))
            {
                chosen_cell->type = TYPE_START;
                break;
            }
            cell = chosen_cell;
            repetitions++;
        }
    }
}

void Maze_Resolve(SEARCH_TYPE type)
{
    if (maze.start_cell == NULL || maze.end_cell == NULL)
        return;

    searching_type = type;
    Maze_Reset(false);

    maze.start_cell->type = TYPE_START;
    maze.end_cell->type = TYPE_END;

    DArray_clear(&stack);
    DArray_append(&stack, maze.start_cell);
}

void Maze_Resolve_async()
{
    switch (searching_type)
    {
    case SEARCH_AMPLITUDE:
        Maze_Resolve_Amplitude_Async();
        break;

    case SEARCH_DEPTH:
        Maze_Resolve_Depth_Async();
        break;

    default:
        break;
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
    maze.start_cell = NULL;
    maze.end_cell = NULL;
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

    if (FloatEquals(0.0f, camera.zoom))
        camera.zoom = 1;
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;

    is_generate = false;
}

void Maze_Logic(void)
{
    Maze_Generate_async();
    Maze_Resolve_async();
    if (!GUI_MouseInGUI())
    {
        // Camera logic
        float mouseDelta = GetMouseWheelMove();
        if (mouseDelta != 0)
        {
            float newZoom = camera.zoom + camera.zoom * mouseDelta * 0.2f;
            if (newZoom <= 0)
                newZoom = 0.01f;
            camera.zoom = newZoom;
        }

        Vector2 thisPos = GetMousePosition();

        Vector2 delta = Vector2Subtract(prevMousePos, thisPos);
        prevMousePos = thisPos;

        if (IsMouseButtonDown(0))
        {
            camera.target = GetScreenToWorld2D(Vector2Add(camera.offset, delta), camera);
        }
        if (IsMouseButtonPressed(1))
        {
            Vector2 clicked = GetScreenToWorld2D(GetMousePosition(), camera);
            Cell *cell = Maze_GetCellWorld(clicked.x, clicked.y);
            Maze_SelectStartEnd(cell);
        }
    }
}

void Maze_Draw(void)
{
    BeginMode2D(camera);
    // Draw cell background
    Cell *cell;
    DrawRectangle(0, 0, maze.sizeX * SIZE_BLOCK, maze.sizeY * SIZE_BLOCK, WHITE);
    for (int x = 0; x < maze.sizeX; x++)
    {
        for (int y = 0; y < maze.sizeY; y++)
        {
            cell = Maze_GetCell(x, y);

            if (cell->type == TYPE_PATH)
            {
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, (Color){0, 255, 255, 200});
            }
            if (cell->type == TYPE_START)
            {
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, (Color){0, 255, 0, 200});
            }
            if (cell->type == TYPE_END)
            {
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, (Color){0, 0, 255, 200});
            }
            if (cell->heat > MAZE_ASYNC_VEL * 1000)
            {
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, (Color){255, 0, 0, cell->heat});
                cell->heat -= MAZE_ASYNC_VEL * 1000;
            }
            // Draw cell borders
            if (cell->neighbours[DIR_UP] == false)
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK + SIZE_BLOCK_BORDER, SIZE_BLOCK_BORDER, BLACK);

            if (cell->neighbours[DIR_LEFT] == false)
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK_BORDER, SIZE_BLOCK + SIZE_BLOCK_BORDER, BLACK);
        }
    }

    EndMode2D();
}

void Maze_Close(void)
{
    free(maze.data);
    maze.sizeX = 0;
    maze.sizeY = 0;
    DArray_free(&stack);
}