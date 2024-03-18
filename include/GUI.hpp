#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include "Maze.hpp"

class GUI
{
private:
    Font font;
    Rectangle GUI_rect;
    float slide_bar_maze_async_vel = 0.1f;
    float slide_bar_maze_width = MAZE_WIDTH;
    float slide_bar_maze_height = MAZE_HEIGHT;
    bool check_box_maze_async = true;
    int combo_box_maze_search_type = 0;

public:
    Maze *maze;

    GUI();
    ~GUI();

    bool MouseInGUI(void);
    void Logic(void);
    void Draw(void);
};

#endif // GUI_H