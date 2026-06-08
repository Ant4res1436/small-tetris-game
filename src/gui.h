#ifndef GUI_H
#define GUI_H

#include "tetris.h"

#define WIDTH 1600
#define HEIGHT 900
#define FPS 250
#define BG_COLOR ((Color) {19, 19, 70, 255})

void GuiLoadTextures(void);
void GuiUnloadTextures(void);


void DrawTetrisGames(TetrisGame *left, TetrisGame *right);
void DrawTetrisGame(TetrisGame *game, Rectangle bounds);
#endif