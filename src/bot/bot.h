#ifndef BOT_H
#define BOT_H

#include <stdint.h>
#include <stdbool.h>
#include "arena.h"
#include "point.h"

#define BOT_ROWS 21
#define BOT_COLUMNS 10
#define BOT_QUEUE_LENGTH 5

#define BOT_PIECE_TYPES 7
#define BOT_PIECE_MINOS 4
#define BOT_ROTATION_TYPES 2
#define BOT_ROTATION_STATES 4
#define BOT_SRS_OFFSETS 5

#define BOT_BOARD_OFFSET_X 3
#define BOT_START_POSITION (BotPoint){(4 + BOT_BOARD_OFFSET_X), 19}
#define BOT_MOVE_RIGHT (BotPoint){1, 0}
#define BOT_MOVE_LEFT (BotPoint){-1, 0}
#define BOT_MOVE_DOWN (BotPoint){0, -1}

typedef enum {
    BOT_EMPTY = 0,
    BOT_I,
    BOT_J,
    BOT_L,
    BOT_O,
    BOT_S,
    BOT_T,
    BOT_Z,
} BotPiece;

typedef enum {
    MOVE_RIGHT = 0,
    MOVE_LEFT,
    MOVE_CW,
    MOVE_CCW,
    MOVE_HOLD,
    MOVE_SOFTDROP,
    MOVE_HARDDROP,
} BotActions;

typedef struct {
    void *parent;
    uint16_t board[BOT_ROWS];
    int32_t score;
    uint32_t lines;
    int32_t combo;
    bool hold;
    bool b2b;
    bool twoCornerRule;
    bool threeCornerRule;
} BotMove;

void AllocateBotMemory(size_t size);
void FreeBotMemory(void);
void GetActions(
    BotActions *actions,
    uint16_t *board,
    BotPiece active,
    BotPiece held,
    BotPiece *queue,
    bool b2b,
    int32_t combo,
    uint8_t maxDepth
    );

#endif