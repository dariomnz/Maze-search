#ifndef Maze_H
#define Maze_H

#include "raylib.h"
#include <vector>
#include <time.h>

#define MAZE_WIDTH 20
#define MAZE_HEIGHT 20
#define SIZE_BLOCK 5
#define SIZE_BLOCK_BORDER 1

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
    bool neighbours[DIR_SIZE];
    bool visited;
    unsigned char heat;
    CELL_TYPE type;
    DIRECTION path_dir;
    bool update_draw;
} Cell;

class GUI;

class Maze
{
private:
    int sizeX;              // Size of x of array data
    int sizeY;              // Size of y of array data
    std::vector<Cell> data; // Array of cells
    Cell *start_cell;       // Pointer to start cell
    Cell *end_cell;         // Pointer to end cell
    std::vector<Cell *> stack;

    Camera2D camera = {0};
    Vector2 prevMousePos = {0};
    bool is_generate = false;
    SEARCH_TYPE searching_type = SEARCH_NULL;
    clock_t t;
    RenderTexture2D maze_texture;
    Rectangle maze_texture_source;

    Cell *GetCell(int x, int y);
    Cell *GetCellWorld(int x, int y);
    bool AnyNeighbour(int x, int y);
    Cell *GetNeighbour(int x, int y, DIRECTION dir);
    DIRECTION GetRandUnvisitedNeighbour(int x, int y);
    DIRECTION GetRandUnvisitedPosibleNeighbour(int x, int y);
    void SetNeighbor(int x, int y, DIRECTION dir, bool value);
    void Reset(bool reset_neighbours);
    void Generate_async(void);
    void SelectStartEnd(Cell *cell);
    void Resolve_Depth_Async(void);
    void Resolve_Amplitude_Async(void);
    void Resolve_async();


public:
    float async_vel = 0.01f;
    GUI *gui;

    Maze();
    ~Maze();

    void Rebuild(int sizeX, int sizeY);
    void Generate(bool async);
    void Resolve(SEARCH_TYPE type);

    void Logic(void);
    void Draw(void);
    void Draw_cell(Cell *cell);
};

#endif // Maze_H