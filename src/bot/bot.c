#include "bot.h"
#include <string.h>

#define MOVE_ARRAY_SIZE 48

typedef struct {
    uint32_t board[BOT_COLUMNS];
} BotMove;

static Arena arena = (Arena){0};

static void Search(BotMove *move, BotPiece active, BotPiece held, BotPiece *queue, uint8_t next, bool b2b, int32_t combo, uint8_t depth);
static void GenerateMoves(BotMove *moves, uint32_t *board, BotPiece *piece);
static void Collision(uint32_t *board, BotPiece piece, uint8_t state);

void AllocateMemory(size_t size) {
    arena = ArenaCreate(size);
}

void FreeMemory(void) {
    ArenaDestroy(&arena);
    arena = (Arena){0};
}

BotActions *GetMoves(uint32_t board[BOT_COLUMNS], BotPiece active, BotPiece held, BotPiece queue[BOT_QUEUE_LENGTH], bool b2b, int32_t combo) {
    BotMove start = {0};
    memcpy(&start.board, board, sizeof(uint32_t) * BOT_COLUMNS);
    Search(&start, active, held, queue, 0, b2b, combo, 1);
}

static void Search(BotMove *move, BotPiece active, BotPiece held, BotPiece *queue, uint8_t next, bool b2b, int32_t combo, uint8_t depth) {
    BotMove moves[MOVE_ARRAY_SIZE];
}

static void GenerateMoves(BotMove *moves, uint32_t *board, BotPiece *piece) {
    memset(moves, 0, sizeof(BotMove) * MOVE_ARRAY_SIZE);
    
}

static void Collision(uint32_t *board, BotPiece piece, uint8_t state) {

}