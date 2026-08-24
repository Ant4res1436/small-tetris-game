#ifndef BOT_H
#define BOT_H

#include "coldclear.h"
#include "tetris.h"

void StartBot(TetrisGame *game);
void RequestBotMove(TetrisGame *game);
void MakeBotMove(TetrisGame *game);
void UpdateBot(TetrisGame *game);
void StopBot(void);

#endif