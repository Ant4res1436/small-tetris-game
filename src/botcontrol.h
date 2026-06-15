#ifndef BOTCONTROL_H
#define BOTCONTROL_H

#include <stdlib.h>
#include "tetris.h"

// Start the bot and pass it BotArgs;
void* StartBotThread(void *arg);
void MakeBestMove(void);

#endif

