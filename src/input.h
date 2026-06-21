#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"
#include <pthread.h>
#include "botcontrol.h"

void SetUpBot(TetrisGame *bot);
void StopBotThread(void);
void HandleInput(TetrisGame *player, TetrisGame *bot, float frametime);

#endif