#include "Window.h"
#include "Maze.h"
#include "GUI.h"
#include "Debug.h"
#include "raylib.h"
#include "raymath.h"


void Window_Init(void)
{
    InitWindow(800, 450, "Search");
    Maze_Init(MAZE_WIDTH, MAZE_HEIGHT);

    GUI_Init();
}

void Window_Logic(void)
{
    Maze_Logic();

    GUI_Logic();
}

void Window_Draw(void)
{
    ClearBackground(BLACK);

    Maze_Draw();

    GUI_Draw();
}

void Window_Close(void)
{
    Maze_Close();
    GUI_Close();
    CloseWindow(); // Close window and OpenGL context
}