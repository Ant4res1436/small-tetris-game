#include "bot.h"
#include "evaluate.h"
#include <string.h>
#include <stdio.h>
#include <float.h>

#define BEAM_SEARCH_SIZE 5
#define MOVE_ARRAY_SIZE 48

static Arena best = (Arena){0};
static Arena nodes = (Arena){0};

static const BotPoint BOT_MINO_TABLE[BOT_PIECE_TYPES][BOT_ROTATION_STATES][BOT_PIECE_MINOS] = {
    {   // I TetrisPiece
        { {-1,0},{0,0},{1,0},{2,0} },
        { {0,1},{0,0},{0,-1},{0,-2} },
        { {-2,0},{-1,0},{0,0},{1,0} },
        { {0,2},{0,1},{0,0},{0,-1} },
    },
    {   // L TetrisPiece
        { {-1,1},{-1,0},{0,0},{1,0} },
        { {1,1},{0,1},{0,0},{0,-1} },
        { {-1,0},{0,0},{1,0},{1,-1} },
        { {0,1},{0,0},{0,-1},{-1,-1} },
    },
    {   // J TetrisPiece
        { {-1,0},{0,0},{1,0},{1,1} },
        { {0,1},{0,0},{0,-1},{1,-1} },
        { {-1,-1},{-1,0},{0,0},{1,0} },
        { {-1,1},{0,1},{0,0},{0,-1} },
    },
    {   // O TetrisPiece
        { {0,0},{1,0},{1,1},{0,1} },
        { {0,0},{1,0},{1,1},{0,1} },
        { {0,0},{1,0},{1,1},{0,1} },
        { {0,0},{1,0},{1,1},{0,1} },
    },
    {   // S TetrisPiece
        { {-1,0},{0,0},{0,1},{1,1} },
        { {0,1},{0,0},{1,0},{1,-1} },
        { {-1,-1},{0,-1},{0,0},{1,0} },
        { {-1,1},{-1,0},{0,0},{0,-1} },
    },
    {   // T TetrisPiece
        { {-1,0},{0,1},{0,0},{1,0} },
        { {0,1},{0,0},{1,0},{0,-1} },
        { {-1,0},{0,-1},{0,0},{1,0} },
        { {0,1},{0,0},{-1,0},{0,-1} },
    },
    {   // Z TetrisPiece
        { {-1,1},{0,1},{0,0},{1,0} },
        { {1,1},{1,0},{0,0},{0,-1} },
        { {-1,0},{0,0},{0,-1},{1,-1} },
        { {-1,-1},{-1,0},{0,0},{0,1} },
    },
};

static const BotPoint BOT_SRS_TABLE_JLSTZ[BOT_ROTATION_TYPES][BOT_ROTATION_STATES][BOT_SRS_OFFSETS] = {
    {
        {
            (BotPoint){ 0, 0},
            (BotPoint){-1, 0},
            (BotPoint){-1,+1},
            (BotPoint){ 0,-2},
            (BotPoint){-1,-2},
        },
        {
            (BotPoint){ 0, 0},
            (BotPoint){+1, 0},
            (BotPoint){+1,-1},
            (BotPoint){ 0,+2},
            (BotPoint){+1,+2},
        },
        {
            (BotPoint){ 0, 0},
            (BotPoint){+1, 0},
            (BotPoint){+1,+1},
            (BotPoint){ 0,-2},
            (BotPoint){+1,-2},
        },
        {
            (BotPoint){ 0, 0},
            (BotPoint){-1, 0},
            (BotPoint){-1,-1},
            (BotPoint){ 0,+2},
            (BotPoint){-1,+2},
        },
    },
    {
        {
            (BotPoint){ 0, 0},
            (BotPoint){+1, 0},
            (BotPoint){+1,+1},
            (BotPoint){ 0,-2},
            (BotPoint){+1,-2},
        },
        {
            (BotPoint){ 0, 0},
            (BotPoint){+1, 0},
            (BotPoint){+1,-1},
            (BotPoint){ 0,+2},
            (BotPoint){+1,+2},
        },
        {
            (BotPoint){ 0, 0},
            (BotPoint){-1, 0},
            (BotPoint){-1,+1},
            (BotPoint){ 0,-2},
            (BotPoint){-1,-2},
        },
        {
            (BotPoint){ 0, 0},
            (BotPoint){-1, 0},
            (BotPoint){-1,-1},
            (BotPoint){ 0,+2},
            (BotPoint){-1,+2},
        },
    }
};

static const BotPoint BOT_SRS_TABLE_I[BOT_ROTATION_TYPES][BOT_ROTATION_STATES][BOT_SRS_OFFSETS] = {
    {
        {
            (BotPoint){ 1, 0},
            (BotPoint){-1, 0},
            (BotPoint){+2, 0},
            (BotPoint){-1,-1},
            (BotPoint){+2,+2},
        },
        {
            (BotPoint){ 0,-1},
            (BotPoint){-1,-1},
            (BotPoint){+2,-1},
            (BotPoint){-1,+1},
            (BotPoint){+2,-2},
        },
        {
            (BotPoint){-1, 0},
            (BotPoint){+1, 0},
            (BotPoint){-2, 0},
            (BotPoint){+1,+1},
            (BotPoint){-2,-2},
        },
        {
            (BotPoint){ 0, 1},
            (BotPoint){+1, 1},
            (BotPoint){-2, 1},
            (BotPoint){+1,-1},
            (BotPoint){-2,+2},
        },
    },
    {
        {
            (BotPoint){ 0,-1},
            (BotPoint){-1,-1},
            (BotPoint){+2,-1},
            (BotPoint){-1,+1},
            (BotPoint){+2,-2},
        },
        {
            (BotPoint){-1, 0},
            (BotPoint){+1, 0},
            (BotPoint){-2, 0},
            (BotPoint){+1,+1},
            (BotPoint){-2,-2},
        },
        {
            (BotPoint){ 0, 1},
            (BotPoint){+1, 1},
            (BotPoint){-2, 1},
            (BotPoint){+1,-1},
            (BotPoint){-2,+2},
        },
        {
            (BotPoint){ 1, 0},
            (BotPoint){-1, 0},
            (BotPoint){+2, 0},
            (BotPoint){-1,-1},
            (BotPoint){+2,+2},
        },
        
    }
};

static void GenMoves(BotState *state, BotState *tempBest, bool hold);
static bool Collision(BotState *state, uint8_t rotation, BotPoint offset);
static void SRS(BotState *state, BotState *tempStates);
static void Place(BotState *state);

void StartBot(BotState *initialState, BotNodes *botNodes) {
    best = ArenaCreate(ARENA_MiB(1));
    nodes = ArenaCreate(ARENA_MiB(50));
    SetDefaultScoring();
    RestartBot(initialState, botNodes);
}

void RestartBot(BotState *initialState, BotNodes *botNodes) {
    ArenaReset(&best);
    ArenaReset(&nodes);
    botNodes->ptr = ArenaAlloc(&nodes, sizeof(initialState));
    memcpy(botNodes->ptr, initialState, sizeof(BotState));
    botNodes->count = 1;
}

void EndBot(void) {
    ArenaDestroy(&best);
    ArenaDestroy(&nodes);
    best = (Arena){0};
    nodes = (Arena){0};
}

void SearchIteration(BotNodes *botNodes) {
    BotNodes tempNodes = {0};
    tempNodes.ptr = botNodes->ptr;

    BotState best[BOT_SEARCH_DEPTH];
    for(int n = 0; n < botNodes->count; n++) {
        if ((botNodes->ptr[n].next + 1) >= BOT_QUEUE_LENGTH) {
            continue;
        }
        memset(best, 0, (sizeof(BotState) * BOT_SEARCH_DEPTH));
        for (int i = 0; i < BOT_SEARCH_DEPTH; i++) {
            best->score = -FLT_MAX;
        }
        GenMoves(&botNodes->ptr[n], best, false);
        GenMoves(&botNodes->ptr[n], best, true);
    }

    botNodes->count = tempNodes.count;
    botNodes->ptr = tempNodes.ptr;
}

void GetBest(BotAction *actions, BotNodes *botNodes) {
}

static void GenMoves(BotState *state, BotState *tempBest, bool hold) {
    BotState newStates[MOVE_ARRAY_SIZE] = {0};
    int count = 0;
    BotState tempState;
    memcpy(&tempState, state, sizeof(BotState));
    if (hold) {
        if (state->held == BOT_EMPTY ) {
            tempState.held = tempState.active;
            tempState.active = tempState.queue[tempState.next++];
        } else {
            BotPiece swap = tempState.active;
            tempState.active = tempState.held;
            tempState.held = swap;
        }
    }

    for (int rot = 0; rot < BOT_ROTATION_STATES; rot++) {
        for (int c = 0; c < BOT_COLUMNS; c++) {
            tempState.position = BOT_START_POSITION;
            tempState.rotation = rot;
            if (Collision(&tempState, rot, (BotPoint){0})) {
                continue;
            }

            count++;
        }
    }

    int countSrs = 0;
    for (int i = 0; i < count; i++) {

    }
    count += countSrs;
}

static bool Collision(BotState *state, uint8_t rotation, BotPoint offset) {
    BotPoint newPosition = state->position;
    PointAdd(&newPosition, offset);
    const BotPoint *minos = BOT_MINO_TABLE[((int)state->active)][rotation];
    BotPoint temp;
    for (int i = 0; i < BOT_PIECE_MINOS; i++) {
        temp = newPosition;
        PointAdd(&temp, minos[i]);
        if (temp.x < 0 || BOT_COLUMNS <= temp.x ||
            temp.y < 0 || temp.y > (sizeof(uint32_t) * 8) ||
            (state->board[temp.x] & (1 << temp.y) != 0)) {
            return true;
        }
    }
    return false;
}

static void SRS(BotState *state, BotState *tempStates) {

}

static void Place(BotState *state) {
    const BotPoint *minos = BOT_MINO_TABLE[((int)state->active)][state->rotation];
    for (int i = 0; i < BOT_PIECE_MINOS; i++) {
        state->board[minos[i].x] |= (1 << minos[i].y);
    }
    state->lines = 0;
    uint32_t lineMask = state->board[0];
    for (int c = 1; c < BOT_COLUMNS; c++) {
        lineMask &= state->board[c];
    }
    if (lineMask == 0) {
        state->combo = 0;
        return;
    }
    uint32_t lower, upper;
    for (int r = (31 - __builtin_clz(lineMask)); r >= __builtin_ctz(lineMask); r--) {
        if ((lineMask & (1 << r)) != 0) {
            state->lines++;
        }
        for (int c = 0; c < BOT_COLUMNS; c++) {
            lower = state->board[c] & ((1 << r) - 1);
            upper = state->board[c] >> (r + 1);
            state->board[c] = (upper << r) | lower;
        }
    }
    state->lines = __builtin_popcount(lineMask);
}
