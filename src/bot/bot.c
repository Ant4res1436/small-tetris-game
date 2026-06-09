#include "bot.h"
#include "evaluate.h"
#include <string.h>

#define BEAM_SEARCH_SIZE 5
#define MOVE_ARRAY_SIZE 48

static Arena arena = (Arena){0};
static BotMove* bestList;

#define EMPTY_ROW   0b1110000000000111
#define FULL_ROW    0b1111111111111111

static const uint16_t MINO_TABLE[BOT_PIECE_TYPES][BOT_ROTATION_STATES][BOT_PIECE_MINOS] =
{
    {
        {
            0b0000000000000000,
            0b0000000000000000,
            0b1111000000000000,
            0b0000000000000000,
        },

        {
            0b0010000000000000,
            0b0010000000000000,
            0b0010000000000000,
            0b0010000000000000,
        },

        {
            0b0000000000000000,
            0b1111000000000000,
            0b0000000000000000,
            0b0000000000000000,
        },

        {
            0b0100000000000000,
            0b0100000000000000,
            0b0100000000000000,
            0b0100000000000000,
        },
    },
    {
        {
            0b0000000000000000,
            0b0000000000000000,
            0b1110000000000000,
            0b1000000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b0100000000000000,
            0b0110000000000000,
        },

        {
            0b0000000000000000,
            0b0010000000000000,
            0b1110000000000000,
            0b0000000000000000,
        },

        {
            0b0000000000000000,
            0b1100000000000000,
            0b0100000000000000,
            0b0100000000000000,
        },
    },
    {

        {
            0b0000000000000000,
            0b0000000000000000,
            0b1110000000000000,
            0b0010000000000000,
        },

        {
            0b0000000000000000,
            0b0110000000000000,
            0b0100000000000000,
            0b0100000000000000,
        },

        {
            0b0000000000000000,
            0b1000000000000000,
            0b1110000000000000,
            0b0000000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b0100000000000000,
            0b1100000000000000,
        },
    },
    {

        {
            0b0000000000000000,
            0b0000000000000000,
            0b0110000000000000,
            0b0110000000000000,
        },

        {
            0b0000000000000000,
            0b0000000000000000,
            0b0110000000000000,
            0b0110000000000000,
        },

        {
            0b0000000000000000,
            0b0000000000000000,
            0b0110000000000000,
            0b0110000000000000,
        },

        {
            0b0000000000000000,
            0b0000000000000000,
            0b0110000000000000,
            0b0110000000000000,
        },
    },
    {

        {
            0b0000000000000000,
            0b0000000000000000,
            0b1100000000000000,
            0b0110000000000000,
        },

        {
            0b0000000000000000,
            0b0010000000000000,
            0b0110000000000000,
            0b0100000000000000,
        },

        {
            0b0000000000000000,
            0b1100000000000000,
            0b0110000000000000,
            0b0000000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b1100000000000000,
            0b1000000000000000,
        },
    },
    {

        {
            0b0000000000000000,
            0b0000000000000000,
            0b1110000000000000,
            0b0100000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b0110000000000000,
            0b0100000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b1110000000000000,
            0b0000000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b1100000000000000,
            0b0100000000000000,
        },
    },
    {

        {
            0b0000000000000000,
            0b0000000000000000,
            0b0110000000000000,
            0b1100000000000000,
        },

        {
            0b0000000000000000,
            0b0100000000000000,
            0b0110000000000000,
            0b0010000000000000,
        },

        {
            0b0000000000000000,
            0b0110000000000000,
            0b1100000000000000,
            0b0000000000000000,
        },

        {
            0b0000000000000000,
            0b1000000000000000,
            0b1100000000000000,
            0b0100000000000000,
        },
    }
};

static void Search(BotMove *previous, BotPiece active, BotPiece held, BotPiece *queue, uint8_t next, bool b2b, int32_t combo, uint8_t depth, uint8_t maxDepth, bool canHold);
static void GenerateMoves(BotMove *best, uint16_t *board, BotPiece piece);
static bool Collision(uint16_t *board, BotPiece piece, BotPoint position, uint8_t state);
static bool Place(BotMove *move, BotPiece piece, BotPoint position, uint8_t state);

void AllocateBotMemory(size_t size) {
    arena = ArenaCreate(size);
}

void FreeBotMemory(void) {
    ArenaDestroy(&arena);
    arena = (Arena){0};
    bestList = NULL;
}

void GetActions(BotActions *actions, uint16_t board[BOT_ROWS], BotPiece active, BotPiece held, BotPiece queue[BOT_QUEUE_LENGTH], bool b2b, int32_t combo, uint8_t maxDepth) {
    ArenaReset(&arena);
    BotMove start = {0};
    start.parent = NULL;
    memcpy(start.board, board, sizeof(uint16_t) * BOT_ROWS);
    bestList = ArenaAlloc(&arena, sizeof(BotMove) * maxDepth);
    for (int i = 0; i < maxDepth; i++) {
        bestList[i].score = INT32_MIN;
    }
    Search(&start, active, held, queue, 0, b2b, combo, 1, maxDepth, true);
}

static void Search(BotMove *previous, BotPiece active, BotPiece held, BotPiece *queue, uint8_t next, bool b2b, int32_t combo, uint8_t depth, uint8_t maxDepth, bool canHold) {
    BotMove *best = ArenaAlloc(&arena, (sizeof(BotMove) * BEAM_SEARCH_SIZE));
    for (int i = 0; i < BEAM_SEARCH_SIZE; i++) {
        best[i].score = INT32_MIN;
    }
    GenerateMoves(best, previous->board, active);
    for (int i = 0; i < BEAM_SEARCH_SIZE; i++) {
        if (best[i].score > bestList[(depth - 1)].score) {
            best[i].parent = previous;
            best[i].score = !canHold;
            bestList[(depth - 1)] = best[i];
        }
    }
    
    
    if (canHold) {
        Search(previous, held, active, queue, next, b2b, combo, depth, maxDepth,false);
    }
}

static void GenerateMoves(BotMove *best, uint16_t *board, BotPiece piece) {
    BotMove moves[MOVE_ARRAY_SIZE] = {0};
    BotPoint position;
    int count = 0;
    for (int s = 0; s < BOT_ROTATION_STATES; s++) {
        for (int c = 0; c < BOT_COLUMNS; c++) {
            // Check if Position is valid
            position.y = BOT_START_POSITION.y;
            position.x = c;
            if (!Collision(board, piece, position, s)) {
                // Add to move list and evaluate
                memcpy(&moves[count].board, board, sizeof(uint16_t) * BOT_ROWS);
                do {
                    position.y--;
                } while (!Collision(board, piece, position, s));
                position.y++;
                Place(&moves[count], piece, position, s);
                // Evaluate and add to best if its a good Move
                Evaluate(&moves[count]);
                for (int i = 0; i < BEAM_SEARCH_SIZE; i++) {
                    if (moves[count].score > best[i].score) {
                        memmove(&best[(i + 1)], &best[i], (sizeof(BotMove) * (BEAM_SEARCH_SIZE - (i + 1))));
                        best[i] = moves[count];
                    }
                }
                count++;
            }
        }
    }
}

static bool Collision(uint16_t *board, BotPiece piece, BotPoint position, uint8_t state) {
    const uint16_t *minos = MINO_TABLE[((int)piece - 1)][state];
    for (int i = 0; i < BOT_PIECE_MINOS; i++) {
        if (minos[i] == 0) {
            continue;
        }
        if ((position.y + i) < 0 || ((minos[i] >> position.x) & board[(position.y + i)]) > 0) {
            return true;
        }
    }
    return false;
}

static bool Place(BotMove *move, BotPiece piece, BotPoint position, uint8_t state) {
    const uint16_t *minos = MINO_TABLE[((int)piece - 1)][state];
    int yPosition;
    int lines = 0;
    for (int i = (BOT_PIECE_MINOS - 1); i >= 0; i--) {
        if (minos[i] == 0) {
            continue;
        }
        yPosition = position.y + i + lines;
        move->board[yPosition] |= (minos[i] >> position.x);
        if (move->board[yPosition] == FULL_ROW) {
            memmove(&move->board[yPosition], &move->board[(yPosition + 1)], BOT_ROWS - ((yPosition - 1) * sizeof(uint16_t)));
            lines++;
        }
    }
    if (lines == 0) {
        move->combo == 0;
    }
}