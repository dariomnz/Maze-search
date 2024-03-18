#include "App.hpp"
#include "Debug.hpp"

App::App() : window(), gui(), maze()
{
    DEBUG_MSG("Init App");
    gui.maze = &maze;
    maze.gui = &gui;
}

App::~App()
{
    DEBUG_MSG("Unload App");
}

void App::Logic(void)
{
    window.Logic();
    maze.Logic();
    gui.Logic();
}

void App::Draw(void)
{
    ClearBackground(BLACK);

    window.Draw();
    maze.Draw();
    gui.Draw();
}