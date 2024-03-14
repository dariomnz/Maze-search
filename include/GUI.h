#ifndef GUI_H
#define GUI_H

#include "raylib.h"

bool GUI_MouseInGUI(void);

void GUI_Init(void);
void GUI_Logic(void);
void GUI_Draw(void);
void GUI_Close(void);

#endif // GUI_H