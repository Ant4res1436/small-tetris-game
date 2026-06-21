#define _POSIX_C_SOURCE 199309L
#include "bot.h"
#include "evaluate.h"
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <immintrin.h>  

#define BEST_BRANCH_SIZE ARENA_MiB(1)
#define MOVE_ARRAY_SIZE 48

static Arena iterations[(BOT_SEARCH_DEPTH + 1)];
static uint32_t currentIteration;
static bool stopSearch;
static Arena bestBranch;

static const BotPoint BOT_MINO_TABLE[BOT_PIECE_TYPES][BOT_ROTATION_STATES][BOT_PIECE_MINOS] = {
    {   // I TetrisPiece
        { {-1,0},{0,0},{1,0},{2,0} },
        { {1,1},{1,0},{1,-1},{1,-2} },
        { {-1,-1},{0,-1},{1,-1},{2,-1} },
        { {0,1},{0,0},{0,-1},{0,-2} },
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

static void GenMoves(BotState *parent, BotState *candidates, bool hold);
static bool Collision(BotState *state, uint8_t rotation, BotPoint offset);
static void SRS(BotState *state);
static void Place(BotState *state);
static int SleepMilliseconds(float milliseconds);
static void PrintBoard(uint32_t *board);

void StartBot(BotState *currentState) {
    SetDefaultScoring();
    int iterationNodes = 1;
    for (int i = 0; i < (BOT_SEARCH_DEPTH + 1); i++) {
        iterations[i] = ArenaCreate(sizeof(BotState) * iterationNodes);
        iterationNodes *= BOT_BEAM_SEARCH_WIDTH;
    }
    bestBranch = ArenaCreate(BEST_BRANCH_SIZE);
    ResetSearch(currentState);
}

void ResetSearch(BotState *currentState) {
    
    for (int i = 0; i < (BOT_SEARCH_DEPTH + 1); i++) {
        ArenaReset(&iterations[i]);
    }
    BotState *rootNode = ArenaAlloc(&iterations[0], sizeof(BotState));
    *rootNode = *currentState;
    currentIteration = 0;
    stopSearch = false;
}

void EndBot(void) {
    for (int i = 0; i < (BOT_SEARCH_DEPTH + 1); i++) {
        ArenaDestroy(&iterations[i]);
    }
    ArenaDestroy(&bestBranch);
}

void SearchIteration(void) {
    if (currentIteration >= BOT_SEARCH_DEPTH) {
        return;
    }

    BotState candidates[BOT_BEAM_SEARCH_WIDTH];
    for (int n = 0; n < (iterations[(currentIteration)].position / sizeof(BotState)); n++) {
        if (stopSearch) {
            SleepMilliseconds(0.1f);
            return;
        }
        if (((BotState*)iterations[currentIteration].memory)[n].next >= BOT_QUEUE_LENGTH) {
            memcpy(ArenaAlloc(&iterations[(currentIteration + 1)], sizeof(BotState)), &((BotState*)iterations[currentIteration].memory)[n], sizeof(BotState));
            continue;
        }
        memset(candidates, 0, (sizeof(BotState) * BOT_BEAM_SEARCH_WIDTH));
        for (int i = 0; i < BOT_BEAM_SEARCH_WIDTH; i++) {
            candidates[i].score = -FLT_MAX;
        }
        GenMoves(&((BotState*)iterations[currentIteration].memory)[n], candidates, false);
        GenMoves(&((BotState*)iterations[currentIteration].memory)[n], candidates, true);
        memcpy(ArenaAlloc(&iterations[(currentIteration + 1)], sizeof(candidates)), candidates, sizeof(candidates));
    }
    
    currentIteration++;
    
}

void GetBest(BotAction *actions) {
    stopSearch = true;
    BotState *best = &((BotState*)iterations[currentIteration].memory)[0];
    for (int n = 1; n < (iterations[currentIteration].position / sizeof(BotState)); n++) {
        //PrintBoard(((BotState*)iterations[currentIteration].memory)[n].board);
        if ((((BotState*)iterations[currentIteration].memory)[n]).score > best->score) {
            best = &((BotState*)iterations[currentIteration].memory)[n];
        }
    }
    BotState *parent;
    while (best->parent != NULL) {
        parent = (BotState*)best->parent;
        if (parent->parent == NULL) {
            break;
        }
        best = parent;
    }
    PrintBoard(best->board);
    int nodes = 0;
    for (int n = 0; n < (BOT_SEARCH_DEPTH + 1); n++) {
        nodes += (iterations[n].position / sizeof(BotState));
    }
    printf("Nodes: %d\n", nodes);

    memcpy(actions, best->actions, sizeof(BotAction) * BOT_ACTIONS_ARRAY_SIZE);
    SleepMilliseconds(0.1f);
}

static void GenMoves(BotState *parent, BotState *candidates, bool hold) {
    BotState newStates[MOVE_ARRAY_SIZE] = {0};
    BotState tempState = *parent;
    tempState.parent = parent;
    int actionCount;
    if (hold) {
        if (parent->held == BOT_EMPTY) {
            tempState.held = tempState.active;
            tempState.active = tempState.queue[tempState.next++];
        } else {
            BotPiece swap = tempState.active;
            tempState.active = tempState.held;
            tempState.held = swap;
        }
    }
    tempState.next++;
    int count = 0;
    for (int rot = 0; rot < BOT_ROTATION_STATES; rot++) {
        for (int c = -1; c < (BOT_COLUMNS + 1); c++) {
            tempState.position.x = c;
            tempState.position.y = BOT_START_POSITION.y;
            tempState.rotation = rot;

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
            for (int i = 0; i < BOT_BEAM_SEARCH_WIDTH; i++) {
                if (newStates[count].score > candidates[i].score) {
                    //memset(newStates[count].actions, 0, (sizeof(BotAction) * MOVE_ARRAY_SIZE));
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
                    for (int j = 0; j < abs((BOT_START_POSITION.x - newStates[count].position.x)); j++) {
                        newStates[count].actions[actionCount] = direction;
                        actionCount++;
                    }
                    if (i < (BOT_BEAM_SEARCH_WIDTH - 1)) {
                        memmove(&candidates[i + 1], &candidates[i], sizeof(BotState) * (BOT_BEAM_SEARCH_WIDTH - (i + 1)));
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
        state->combo = -1;
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