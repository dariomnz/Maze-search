#include "Window.h"
#include "Maze.h"
#include "Debug.h"
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

Camera2D camera = {0};
Vector2 prevMousePos = {0};

void Window_Init(void)
{
    InitWindow(800, 450, "Search");
    Maze_Init(MAZE_X, MAZE_Y);
    camera.zoom = 1;
    camera.offset.x = GetScreenWidth() / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;

    prevMousePos = GetMousePosition();
}

void Window_Logic(void)
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

    if (IsKeyPressed(KEY_LEFT))
        camera.rotation += 10;
    else if (IsKeyPressed(KEY_RIGHT))
        camera.rotation -= 10;

    Maze_Logic();
}

void Window_Draw(void)
{
    ClearBackground(BLACK);
    BeginMode2D(camera);

    Maze_Draw();

    EndMode2D();

    if (GuiButton((Rectangle){10, 10, 100, 30}, "Generate maze"))
    {
        Maze_Generate(false);
    }
    if (GuiButton((Rectangle){10, 40, 100, 30}, "Generate maze async"))
    {
        Maze_Generate(true);
    }
    DrawFPS(0, 0);
}

void Window_Close(void)
{
    Maze_Close();
    CloseWindow(); // Close window and OpenGL context
}