#include "Window.h"
#include "Maze.h"
#include "GUI.h"
#include "Debug.h"
#include "raylib.h"
#include "raymath.h"

Camera2D camera = {0};
Vector2 prevMousePos = {0};

void Window_Init(void)
{
    InitWindow(800, 450, "Search");
    Maze_Init(MAZE_WIDTH, MAZE_HEIGHT);
    camera.zoom = 1;
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;

    prevMousePos = GetMousePosition();

    GUI_Init();
}

void Window_Logic(void)
{
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
            camera.target = GetScreenToWorld2D(Vector2Add(camera.offset, delta), camera);
    }
    Maze_Logic();
}

void Window_Draw(void)
{
    ClearBackground(BLACK);
    BeginMode2D(camera);

    Maze_Draw();

    EndMode2D();

    GUI_Draw();
}

void Window_Close(void)
{
    Maze_Close();
    GUI_Close();
    CloseWindow(); // Close window and OpenGL context
}