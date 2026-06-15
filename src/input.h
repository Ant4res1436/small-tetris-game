#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"
#include <pthread.h>
#include "botcontrol.h"

void HandleInput(TetrisGame *player, TetrisGame *bot, pthread_t *botThread);

#endif