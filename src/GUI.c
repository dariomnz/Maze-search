#include "GUI.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Maze.h"
#define GUI_WIDTH 0.2f
#define GUI_HEIGHT 1.0f

float maze_async_vel = 0.1f;
float maze_width = MAZE_WIDTH;
float maze_height = MAZE_HEIGHT;
bool maze_async = true;
Font font;
RenderTexture textureGUI;
Rectangle GUI_rect;

bool GUI_MouseInGUI()
{
    return CheckCollisionPointRec(GetMousePosition(), GUI_rect);
}

void GUI_Init(void)
{
    font = LoadFontEx("./fonts/Monaco.ttf", 82, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    GuiSetFont(font);
    int width = GetScreenWidth() * GUI_WIDTH;
    int height = GetScreenHeight() * GUI_HEIGHT;
    GUI_rect = (Rectangle){0, 0, width, height};
}

void GUI_Logic(void)
{
}

char label_vel[20];
char label_xy[20];
void GUI_Draw(void)
{
    int x = 20;
    int y = 20;
    int w = 0;
    int h = 0;
    DrawRectangleRec(GUI_rect, RAYWHITE);
    DrawFPS(x, 0);
    if (GuiButton((Rectangle){x, y += h, w = 100, h = 30}, "Create maze"))
    {
        Maze_Init((int)maze_width, (int)maze_height);
    }
    if (GuiButton((Rectangle){x, y += h, w = 100, h = 30}, "Generate maze"))
    {
        Maze_Generate(maze_async);
    }
    sprintf(label_vel, " Velocity: %.02f", maze_async_vel);
    GuiLabel((Rectangle){x, y += h, w = 120, h = 20}, label_vel);
    if (GuiSliderBar((Rectangle){x + 15, y += h, w = 70, h = 15}, "0.1", "1", &maze_async_vel, 0.1, 1))
    {
        Maze_SetMAZE_ASYNC_VEL(maze_async_vel * 0.01f);
    }

    sprintf(label_xy, " Width:Height %d:%d", (int)maze_width, (int)maze_height);
    GuiLabel((Rectangle){x - 10, y += h, w = 150, h = 20}, label_xy);
    GuiSliderBar((Rectangle){x + 15, y += h, w = 70, h = 15}, "10", "200", &maze_width, 10, 200);
    GuiSliderBar((Rectangle){x + 15, y += h, w = 70, h = 15}, "10", "200", &maze_height, 10, 200);
    GuiCheckBox((Rectangle){x + 15, y += h, w = 20, h = 20}, "Async", &maze_async);
}

void GUI_Close(void)
{
    UnloadFont(font);
}