#ifndef App_H
#define App_H

#include "Window.hpp"
#include "GUI.hpp"
#include "Maze.hpp"

class App
{
private:
    Window window;
    GUI gui;
    Maze maze;

public:
    App();
    ~App();
    void Logic(void);
    void Draw(void);
};

#endif // App_H