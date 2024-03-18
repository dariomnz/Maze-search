#include "Window.hpp"
#include "Maze.hpp"
#include "GUI.hpp"
#include "Debug.hpp"
#include "raylib.h"
#include "raymath.h"

Window::Window(void)
{
    DEBUG_MSG("Init Window");
    InitWindow(800, 450, "Search");
}

Window::~Window()
{
    CloseWindow(); // Close window and OpenGL context
    DEBUG_MSG("Unload window");
}

void Window::Logic(void)
{
}

void Window::Draw(void)
{
    ClearBackground(BLACK);
}