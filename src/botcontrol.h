#ifndef BOTCONTROL_H
#define BOTCONTROL_H

#include <stdlib.h>
#include "tetris.h"

typedef struct {
    TetrisGame *game;
    float piecesPerSecond;
} BotArgs;

// Start the bot and pass it BotArgs;
void* StartBotThread(void* arg);

#endif

