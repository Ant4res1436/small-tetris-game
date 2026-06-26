#ifndef BOT_H
#define BOT_H

#include <stdint.h>
#include <stdbool.h>
#include "arena.h"
#include "point.h"

#define BOT_SEARCH_DEPTH 5
#define BOT_BEAM_SEARCH_WIDTH 20

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

#define BOT_ACTIONS_ARRAY_SIZE 16

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
    MOVE_HARDDROP = 0,
    MOVE_RIGHT,
    MOVE_LEFT,
    MOVE_CW,
    MOVE_CCW,
    MOVE_HOLD,
    MOVE_SOFTDROP,
} BotAction;

typedef struct {
    void *parent;
    BotAction actions[BOT_ACTIONS_ARRAY_SIZE];
    uint32_t board[BOT_COLUMNS];
    BotPiece active;
    BotPoint position;
    uint8_t rotation;
    bool tspin;
    BotPiece held;
    BotPiece queue[BOT_QUEUE_LENGTH];
    uint8_t next;
    uint8_t lines;
    int8_t combo;
    bool b2b;
    float score;
} BotState;

typedef struct {
    BotState* ptr;
    uint32_t count; 
} BotNodes;

void StartBot(BotState *currentState);
void ResetSearch(BotState *currentState);
void EndBot(void);
void SearchIteration(void);
void GetBest(BotAction *actions);
#endif