#include "GUI.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Maze.hpp"
#include "Debug.hpp"
#include <cassert>
#define GUI_WIDTH 0.2f
#define GUI_HEIGHT 1.0f


bool GUI::MouseInGUI()
{
    return CheckCollisionPointRec(GetMousePosition(), GUI_rect);
}

GUI::GUI()
{
    DEBUG_MSG("Init GUI");
    font = LoadFontEx("./fonts/MonaspaceNeon-Bond.otf", 82, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    GuiSetFont(font);
    float width = GetScreenWidth() * GUI_WIDTH;
    float height = GetScreenHeight() * GUI_HEIGHT;
    GUI_rect = (Rectangle){0, 0, width, height};
}
GUI::~GUI()
{
    UnloadFont(font);
    DEBUG_MSG("Unload GUI");
}

void GUI::Logic(void)
{
}

char label_vel[40];
char label_xy[40];
void GUI::Draw(void)
{
    float x = 20;
    float y = 20;
    float w = 0;
    float h = 0;
    DrawRectangleRec(GUI_rect, RAYWHITE);
    DrawFPS(x, 0);
    if (GuiButton((Rectangle){x, y += h, w = 100, h = 30}, "Create maze"))
    {
        maze->Rebuild((int)slide_bar_maze_width, (int)slide_bar_maze_height);
    }
    if (GuiButton((Rectangle){x, y += h, w = 100, h = 30}, "Generate maze"))
    {
        maze->Generate(check_box_maze_async);
    }
    sprintf(label_vel, " Velocity: %.02f", slide_bar_maze_async_vel);
    GuiLabel((Rectangle){x, y += h, w = 120, h = 20}, label_vel);
    if (GuiSliderBar((Rectangle){x + 15, y += h, w = 70, h = 15}, "0.1", "1", &slide_bar_maze_async_vel, 0.1, 1))
    {
        maze->async_vel = slide_bar_maze_async_vel * 0.01f;
    }

    sprintf(label_xy, " Width:Height %d:%d", (int)slide_bar_maze_width, (int)slide_bar_maze_height);
    GuiLabel((Rectangle){x - 10, y += h, w = 150, h = 20}, label_xy);
    GuiSliderBar((Rectangle){x + 15, y += h, w = 70, h = 15}, "10", "200", &slide_bar_maze_width, 10, 200);
    GuiSliderBar((Rectangle){x + 15, y += h, w = 70, h = 15}, "10", "200", &slide_bar_maze_height, 10, 200);
    GuiCheckBox((Rectangle){x + 15, y += h, w = 20, h = 20}, "Async", &check_box_maze_async);

    GuiComboBox((Rectangle){x, y += h, w = 100, h = 30}, "Amplitude;Depth", &combo_box_maze_search_type);
    if (GuiButton((Rectangle){x, y += h, w = 100, h = 30}, "Resolve maze"))
    {
        maze->Resolve((SEARCH_TYPE)(combo_box_maze_search_type + 1));
    }
}