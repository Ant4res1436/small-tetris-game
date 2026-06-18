#define _POSIX_C_SOURCE 199309L
#include "bot.h"
#include "evaluate.h"
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <immintrin.h>  

#define BEAM_SEARCH_SIZE 3
#define MOVE_ARRAY_SIZE 48

static Arena best = (Arena){0};
static Arena nodes = (Arena){0};
static bool stopSearch;

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

static const BotPoint BOT_ANCHOR_I_OFFSETS[BOT_ROTATION_STATES] = {
    (BotPoint){0, 0},
    (BotPoint){1, 0},
    (BotPoint){1, -1},
    (BotPoint){0, -1},
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

static void GenMoves(BotState *state, BotState *candidates, bool hold);
static bool Collision(BotState *state, uint8_t rotation, BotPoint offset);
static void SRS(BotState *state);
static void Place(BotState *state);
static int SleepMilliseconds(float milliseconds);
static void PrintBoard(uint32_t *board);

void StartBot(BotState *initialState, BotNodes *botNodes) {
    best = ArenaCreate(ARENA_MiB(1));
    nodes = ArenaCreate(ARENA_MiB(100));
    SetDefaultScoring();
    RestartBot(initialState, botNodes);

}

void RestartBot(BotState *initialState, BotNodes *botNodes) {
    ArenaReset(&best);
    ArenaReset(&nodes);
    botNodes->ptr = ArenaAlloc(&nodes, sizeof(initialState));
    memcpy(botNodes->ptr, initialState, sizeof(BotState));
    botNodes->count = 1;
    stopSearch = false;
}

void EndBot(void) {
    stopSearch = true;
    ArenaDestroy(&best);
    ArenaDestroy(&nodes);
    best = (Arena){0};
    nodes = (Arena){0};
}

void SearchIteration(BotNodes *botNodes) {
    BotNodes tempNodes = {0};
    tempNodes.ptr = botNodes->ptr + botNodes->count;

    BotState best[BOT_SEARCH_DEPTH];
    for(int n = 0; n < botNodes->count; n++) {
        while (stopSearch) {
            SleepMilliseconds(1);
            return;
        }
        if ((botNodes->ptr[n].next + 1) >= BOT_QUEUE_LENGTH) {
            stopSearch = true;
            break;
        }
        //printf("aa\n");
        memset(best, 0, (sizeof(BotState) * BOT_SEARCH_DEPTH));
        for (int i = 0; i < BOT_SEARCH_DEPTH; i++) {
            best->score = -FLT_MAX;
        }
        GenMoves(&botNodes->ptr[n], best, false);
        GenMoves(&botNodes->ptr[n], best, true);
        if (ArenaAlloc(&nodes, sizeof(best)) == NULL) {
            stopSearch = true;
            break;
        }
        tempNodes.count += BOT_SEARCH_DEPTH;
        
    }

    botNodes->count = tempNodes.count;
    botNodes->ptr = tempNodes.ptr;
}

void GetBest(BotAction *actions, BotState *root) {
    BotState candidates[BOT_SEARCH_DEPTH] = {0};
    for (int i = 0; i < BOT_SEARCH_DEPTH; i++) {
        candidates->score = -FLT_MAX;
    }

    GenMoves(root, candidates, false);
    GenMoves(root, candidates, true);

    BotState *best = &candidates[0];
    for (int i = 1; i < BOT_SEARCH_DEPTH; i++) {
        if (candidates[i].score > best->score) {
            best = &candidates[i];
        }
    }
    PrintBoard(best->board);
    printf("Best X: %d\n", best->position.x);
    memcpy(actions, best->actions, sizeof(BotAction) * BOT_ACTIONS_ARRAY_SIZE);
}

static void GenMoves(BotState *state, BotState *candidates, bool hold) {
    BotState newStates[MOVE_ARRAY_SIZE] = {0};
    BotState tempState = *state;
    tempState.parent = state;
    int actionCount;
    if (hold) {
        if (state->held == BOT_EMPTY) {
            tempState.held = tempState.active;
            tempState.active = tempState.queue[tempState.next++];
        } else {
            BotPiece swap = tempState.active;
            tempState.active = tempState.held;
            tempState.held = swap;
        }
    }
    int count = 0;
    for (int rot = 0; rot < BOT_ROTATION_STATES; rot++) {
        for (int c = -1; c < (BOT_COLUMNS + 1); c++) {
            tempState.position.x = c;
            tempState.position.y = BOT_START_POSITION.y;
            tempState.rotation = rot;
            if (tempState.active = BOT_I) {
                PointAdd(&tempState.position, BOT_ANCHOR_I_OFFSETS[rot]);
            }

            if (Collision(&tempState, rot, (BotPoint){0})) {
                continue;
            }
            newStates[count] = tempState;
            while (!Collision(&newStates[count], rot, BOT_MOVE_DOWN)) 
            {
                newStates[count].position.y--;
            };
            Place(&newStates[count]);
            Evaluate(&newStates[count]);
            for (int i = 0; i < BOT_SEARCH_DEPTH; i++) {
                if (newStates[count].score > candidates[i].score) {
                    actionCount = 0;
                    if (hold) {
                        newStates[count].actions[actionCount] = MOVE_HOLD;
                        actionCount++;
                    }
                    switch (rot) {
                        case 1:
                            newStates[count].actions[actionCount] = MOVE_CW;
                            actionCount++;
                            break;
                        case 2:
                            newStates[count].actions[actionCount] = MOVE_CW;
                            actionCount++;
                            newStates[count].actions[actionCount] = MOVE_CW;
                            actionCount++;
                            break;
                        case 3:
                            newStates[count].actions[actionCount] = MOVE_CCW;
                            actionCount++;
                            break;
                        default:
                            break;
                    }
                    BotAction direction = MOVE_RIGHT;
                    if (newStates[count].position.x < BOT_START_POSITION.x) {
                        direction = MOVE_LEFT;
                    }
                    for (int i = 0; i < abs((BOT_START_POSITION.x - newStates[count].position.x)); i++) {
                        newStates[count].actions[actionCount] = direction;
                        actionCount++;
                    }
                    if (i < (BOT_SEARCH_DEPTH - 1)) {
                        memmove(&candidates[i + 1], &candidates[i], sizeof(BotState) * (BOT_SEARCH_DEPTH - (i + 1)));
                    }
                    candidates[i] = newStates[count];
                    break;
                }
            }
            count++;
        }
    }
}

static bool Collision(BotState *state, uint8_t rotation, BotPoint offset) {
    BotPoint newPosition = state->position;
    PointAdd(&newPosition, offset);
    const BotPoint *minos = BOT_MINO_TABLE[((int)state->active - 1)][rotation];
    BotPoint temp;
    for (int i = 0; i < BOT_PIECE_MINOS; i++) {
        temp = newPosition;
        PointAdd(&temp, minos[i]);
        if (temp.x < 0 || BOT_COLUMNS <= temp.x ||
            temp.y < 0 || temp.y > (sizeof(uint32_t) * 8) ||
            (state->board[temp.x] & (1 << temp.y)) != 0) {
            return true;
        }
    }
    return false;
}

static void SRS(BotState *state) {

}

static void Place(BotState *state) {
    const BotPoint *minos = BOT_MINO_TABLE[((int)state->active - 1)][state->rotation];
    BotPoint temp;
    for (int i = 0; i < BOT_PIECE_MINOS; i++) {
        temp = state->position;
        PointAdd(&temp, minos[i]);
        state->board[temp.x] |= (1 << temp.y);
    }
    uint32_t lineMask = state->board[0];
    for (int c = 1; c < BOT_COLUMNS; c++) {
        lineMask &= state->board[c];
    }
    if (lineMask == 0) {
        state->combo = 0;
        state->lines = 0;
        return;
    }
    uint32_t keepMask = ~lineMask;
    for (int c = 0; c < BOT_COLUMNS; c++) {
        state->board[c] = _pext_u32(state->board[c], keepMask);
    }
    state->lines = __builtin_popcount(lineMask);
    state->combo++;
}

static int SleepMilliseconds(float milliseconds) {
    int seconds = 0;
    if (milliseconds >= 1000.0f) {
        seconds = (int)(milliseconds / 1000);
        milliseconds -= (seconds * 1000.0f);
    }
    struct timespec sleep = {seconds, (int)(milliseconds * 1000000.0f)};
    return nanosleep(&sleep, NULL);
}

static void PrintBoard(uint32_t *board) {
    printf("\n");
    for (int r = (BOT_ROWS - 1); r >= 0; r--) {
        for (int c = 0; c < BOT_COLUMNS; c++) {
            if ((board[c] & (1 << r)) == 0) {
                printf("0");
            } else {
                printf("1");
            }
        }
        printf("\n");
    }
    printf("\n");
}