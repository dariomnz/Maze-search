#include "Maze.hpp"
#include "Debug.hpp"
// #include "stdlib.h"
#include "cstring"
// #include <time.h>
#include "raymath.h"
#include "GUI.hpp"

Camera2D camera = {0};
Vector2 prevMousePos = {0};
bool is_generate = false;
SEARCH_TYPE searching_type = SEARCH_NULL;
clock_t t;

bool EqualCell(Cell *cell1, Cell *cell2)
{
    if (cell1 == NULL || cell2 == NULL)
        return false;
    return cell1->x == cell2->x && cell1->y == cell2->y;
}

Cell *Maze::GetCell(int x, int y)
{
    if (x < 0 || x >= sizeX)
        DEBUG_ERROR("Maze x (" << x << ") out of limits (" << sizeX << ")");
    if (y < 0 || y >= sizeY)
        DEBUG_ERROR("Maze x (" << y << ") out of limits (" << sizeY << ")");
    return &data[x * sizeY + y];
}

Cell *Maze::GetCellWorld(int x, int y)
{
    return GetCell(x / SIZE_BLOCK, y / SIZE_BLOCK);
}

bool Maze::AnyNeighbour(int x, int y)
{
    Cell *cell = GetCell(x, y);
    bool output;
    for (int i = 0; i < DIR_SIZE; i++)
    {
        output |= cell->neighbours[i];
    }
    return output;
}

Cell *Maze::GetNeighbour(int x, int y, DIRECTION dir)
{
    switch (dir)
    {
    case DIR_UP:
        if (y == 0)
            return NULL;
        return GetCell(x, y - 1);
    case DIR_DOWN:
        if (y == sizeY - 1)
            return NULL;
        return GetCell(x, y + 1);
    case DIR_LEFT:
        if (x == 0)
            return NULL;
        return GetCell(x - 1, y);
    case DIR_RIGHT:
        if (x == sizeX - 1)
            return NULL;
        return GetCell(x + 1, y);
    default:
        DEBUG_ERROR("incorrect dir " << dir);
        return NULL;
    }
}

DIRECTION Maze::GetRandUnvisitedNeighbour(int x, int y)
{
    Cell *cell_neig;
    int repetition = 0;
    for (int i = rand() % DIR_SIZE; repetition < DIR_SIZE; i = (i + 1) % DIR_SIZE)
    {
        cell_neig = GetNeighbour(x, y, (DIRECTION)i);
        if (cell_neig == NULL)
            continue;
        if (!cell_neig->visited)
            return (DIRECTION)i;
        repetition++;
    }
    return DIR_SIZE;
}

DIRECTION Maze::GetRandUnvisitedPosibleNeighbour(int x, int y)
{
    Cell *cell_neig;
    int repetition = 0;
    for (int i = rand() % DIR_SIZE; repetition < DIR_SIZE; i = (i + 1) % DIR_SIZE)
    {
        cell_neig = GetNeighbour(x, y, (DIRECTION)i);
        if (cell_neig == NULL || GetCell(x, y)->neighbours[i] == false)
            continue;
        if (!cell_neig->visited)
            return (DIRECTION)i;
        repetition++;
    }
    return DIR_SIZE;
}

DIRECTION OpositeDir(DIRECTION dir)
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
        DEBUG_ERROR("incorrect dir " << dir);
        return DIR_SIZE;
    }
}

void Maze::SetNeighbor(int x, int y, DIRECTION dir, bool value)
{
    Cell *cell = NULL, *cell_neig = NULL;
    DIRECTION dir_neig = OpositeDir(dir);

    cell = GetCell(x, y);
    cell_neig = GetNeighbour(x, y, dir);
    if (cell_neig == NULL)
        return;
    cell->neighbours[dir] = value;
    cell_neig->neighbours[dir_neig] = value;
}

void Maze::Reset(bool reset_neighbours)
{
    Cell *cell;
    for (int x = 0; x < sizeX; x++)
    {
        for (int y = 0; y < sizeY; y++)
        {
            cell = GetCell(x, y);
            cell->visited = false;
            cell->type = TYPE_NULL;
            cell->heat = 0;
            if (reset_neighbours)
                for (int i = 0; i < DIR_SIZE; i++)
                {
                    cell->neighbours[i] = false;
                }
        }
    }
}

void Maze::Generate_async(void)
{
    if (!is_generate)
    {
        return;
    }

    Cell *cell, *chosen_cell;
    DIRECTION dir;
    for (int i = 0; i < async_vel * sizeX * sizeY && stack.size() > 0; i++)
    {
        // Pop cell
        cell = stack.back();
        stack.pop_back();

        // Check neighbours
        dir = GetRandUnvisitedNeighbour(cell->x, cell->y);
        if (dir != DIR_SIZE)
        {
            // Push cell
            stack.push_back(cell);
            // Choose one
            SetNeighbor(cell->x, cell->y, dir, true);
            chosen_cell = GetNeighbour(cell->x, cell->y, dir);
            chosen_cell->visited = true;
            stack.push_back(chosen_cell);
            chosen_cell->heat = 255;
        }
    }
    if (stack.size() <= 0)
    {
        is_generate = false;

        for (int i = 0; i < sizeX * sizeY * 0.05f; i++)
        {
            int x = rand() % sizeX;
            int y = rand() % sizeY;
            cell = GetCell(x, y);
            dir = (DIRECTION)(rand() % DIR_SIZE);
            chosen_cell = GetNeighbour(cell->x, cell->y, dir);
            if (chosen_cell)
            {
                SetNeighbor(x, y, dir, true);
                cell->heat = 255;
            }
        }

        double time_taken = ((double)(clock() - t)) / CLOCKS_PER_SEC; // in seconds
        DEBUG_MSG("Generate() took " << time_taken << " seconds to execute");
    }
}
void Maze::Generate(bool async)
{
    DEBUG_MSG("Maze::Generate() started");

    // Reset
    Reset(true);

    Cell *cell;
    int x = rand() % sizeX;
    int y = rand() % sizeY;
    cell = GetCell(x, y);
    cell->visited = true;
    stack.clear();
    stack.push_back(cell);
    t = clock();
    is_generate = true;
    if (!async)
    {
        while (stack.size() > 0)
        {
            Generate_async();
        }
    }
}

void Maze::SelectStartEnd(Cell *cell)
{
    if (cell == NULL)
        return;

    if (start_cell == NULL)
    {
        start_cell = cell;
        cell->type = TYPE_START;
    }
    else if (end_cell == NULL)
    {
        end_cell = cell;
        cell->type = TYPE_END;
    }
    else if (start_cell != NULL)
    {
        start_cell->type = TYPE_NULL;

        start_cell = cell;
        cell->type = TYPE_START;
        end_cell->type = TYPE_NULL;
        end_cell = NULL;
    }
}

void Maze::Resolve_Depth_Async(void)
{
    DIRECTION dir;
    bool find = false;
    Cell *cell, *chosen_cell;

    for (int i = 0; i < async_vel * sizeX * sizeY && stack.size() > 0; i++)
    {
        // Pop cell
        cell = stack.back();
        stack.pop_back();

        dir = GetRandUnvisitedPosibleNeighbour(cell->x, cell->y);
        if (dir != DIR_SIZE)
        {
            // Push cell
            stack.push_back(cell);
            // Choose one
            chosen_cell = GetNeighbour(cell->x, cell->y, dir);
            chosen_cell->visited = true;
            stack.push_back(chosen_cell);
            chosen_cell->heat = 255;
            if (EqualCell(chosen_cell, end_cell))
            {
                find = true;
            }
        }
    }
    if (stack.size() <= 0 || find == true)
    {
        searching_type = SEARCH_NULL;
        // Mark path
        for (auto it = stack.begin(); it != stack.end(); ++it)
        {
            if (EqualCell(*it, start_cell))
                (*it)->type = TYPE_START;
            else if (EqualCell(*it, end_cell))
                (*it)->type = TYPE_END;
            else
                (*it)->type = TYPE_PATH;
        }
    }
}

void Maze::Resolve_Amplitude_Async(void)
{
    bool find = false;
    Cell *cell, *chosen_cell;

    for (int i = 0; i < async_vel * sizeX * sizeY && stack.size() > 0; i++)
    {
        // Pop cell
        cell = stack.front();
        stack.erase(stack.begin());

        for (int i = 0; i < DIR_SIZE; i++)
        {
            chosen_cell = GetNeighbour(cell->x, cell->y, (DIRECTION)i);
            if (chosen_cell == NULL || GetCell(cell->x, cell->y)->neighbours[i] == false)
                continue;
            if (!chosen_cell->visited)
            {
                chosen_cell->path_dir = OpositeDir((DIRECTION)i);
                chosen_cell->visited = true;
                stack.push_back(chosen_cell);
                chosen_cell->heat = 255;
                if (EqualCell(chosen_cell, end_cell))
                {
                    find = true;
                }
            }
        }
    }
    if (stack.size() <= 0 || find == true)
    {
        searching_type = SEARCH_NULL;
        // Mark path
        cell = end_cell;
        int repetitions = 0;
        while (repetitions < sizeX * sizeY)
        {
            if (EqualCell(cell, end_cell))
                cell->type = TYPE_END;
            else
                cell->type = TYPE_PATH;

            chosen_cell = GetNeighbour(cell->x, cell->y, cell->path_dir);
            if (chosen_cell == NULL)
                break;

            if (EqualCell(chosen_cell, start_cell))
            {
                chosen_cell->type = TYPE_START;
                break;
            }
            cell = chosen_cell;
            repetitions++;
        }
    }
}

void Maze::Resolve(SEARCH_TYPE type)
{
    if (start_cell == NULL || end_cell == NULL)
        return;

    searching_type = type;
    Reset(false);

    start_cell->type = TYPE_START;
    end_cell->type = TYPE_END;

    stack.clear();
    stack.push_back(start_cell);
}

void Maze::Resolve_async()
{
    switch (searching_type)
    {
    case SEARCH_AMPLITUDE:
        Resolve_Amplitude_Async();
        break;

    case SEARCH_DEPTH:
        Resolve_Depth_Async();
        break;

    default:
        break;
    }
}

Maze::Maze()
{
    DEBUG_MSG("Init Maze " << MAZE_WIDTH << " " << MAZE_HEIGHT);
    Rebuild(MAZE_WIDTH, MAZE_HEIGHT);
}

void Maze::Rebuild(int sizeX_, int sizeY_)
{
    sizeX = sizeX_;
    sizeY = sizeY_;
    data.clear();
    data.resize(sizeX * sizeY);
    start_cell = NULL;
    end_cell = NULL;

    // Set x and y
    Cell *cell;
    for (int x = 0; x < sizeX; x++)
    {
        for (int y = 0; y < sizeY; y++)
        {
            cell = GetCell(x, y);
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

void Maze::Logic(void)
{
    Generate_async();
    Resolve_async();
    if (!gui->MouseInGUI())
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
            Cell *cell = GetCellWorld(clicked.x, clicked.y);
            SelectStartEnd(cell);
        }
    }
}

void Maze::Draw(void)
{
    BeginMode2D(camera);
    // Draw cell background
    Cell *cell;
    DrawRectangle(0, 0, sizeX * SIZE_BLOCK, sizeY * SIZE_BLOCK, WHITE);
    for (int x = 0; x < sizeX; x++)
    {
        for (int y = 0; y < sizeY; y++)
        {
            cell = GetCell(x, y);

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
            if (cell->heat > async_vel * 1000)
            {
                DrawRectangle(x * SIZE_BLOCK, y * SIZE_BLOCK, SIZE_BLOCK, SIZE_BLOCK, (Color){255, 0, 0, cell->heat});
                cell->heat -= async_vel * 1000;
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

Maze::~Maze(void)
{
    DEBUG_MSG("Unload maze");
    sizeX = 0;
    sizeY = 0;
}