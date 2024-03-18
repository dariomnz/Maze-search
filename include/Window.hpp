#ifndef Window_H
#define Window_H

#include "raylib.h"
#include "GUI.hpp"
#include "Maze.hpp"

class Window
{
private:
public:
    Window(void);
    ~Window();
    void Logic(void);
    void Draw(void);
};

#endif // Window_H