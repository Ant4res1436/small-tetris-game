#include "bot.h"
#include "evaluate.h"
#include <string.h>
#include <stdio.h>

#define BEAM_SEARCH_SIZE 5
#define MOVE_ARRAY_SIZE 48

static Arena arena = (Arena){0};

void AllocateBotMemory(size_t size) {
    arena = ArenaCreate(size);
}

void FreeBotMemory(void) {
    ArenaDestroy(&arena);
    arena = (Arena){0};
}

int Search(BotState *currentState) {
    return 0;
}
void GetBest(BotAction *actions, BotState *currentState) {

}
void PrintBoard(uint32_t *board) {
    for (int c = 0; c < BOT_COLUMNS; c++) {
        
    }
}
