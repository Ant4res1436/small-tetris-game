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

#define BOT_START_POSITION (BotPoint){4, 19}
#define BOT_MOVE_RIGHT (BotPoint){1, 0}
#define BOT_MOVE_LEFT (BotPoint){-1, 0}
#define BOT_MOVE_DOWN (BotPoint){0, -1}

typedef enum {
    EMPTY = 0,
    I,
    J,
    L,
    O,
    S,
    T,
    Z,
} BotPiece;

typedef enum {
    RIGHT = 0,
    LEFT,
    CW,
    CCW,
    HOLD,
    SOFTDROP,
    HARDDROP,
} BotActions;

typedef struct {
    BotPiece piece;
    BotPoint position;
    uint8_t state;
    bool rotatedLast;
    bool kickException;
} BotMove;

void AllocateMemory(size_t size);

BotActions *GetMoves(
    uint32_t board[BOT_COLUMNS],
    BotPiece active,
    BotPiece held,
    BotPiece queue[BOT_QUEUE_LENGTH],
    bool b2b,
    int32_t combo
    );

void FreeMemory();

#endif