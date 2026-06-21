#define _POSIX_C_SOURCE 199309L
#include "botcontrol.h"
#include "bot/bot.h"
#include <time.h>
#include <stdio.h>
#include <pthread.h>

static bool pauseSearch;

static void GenBitboard(TetrisGame *game, BotState *state);
static void CarryPieces(TetrisGame *game, BotState *state);
static void PrintBoard(uint32_t *board);
static int SleepMilliseconds(float milliseconds);

void* StartBotThread(void *arg) {
    
    TetrisGame *game = (TetrisGame*)arg;
    BotState currentState = {0};

    currentState.combo = game->combo;
    currentState.b2b = game->backToBack;
    GenBitboard(game, &currentState);
    CarryPieces(game, &currentState);
    StartBot(&currentState);
    pauseSearch = false;

    pthread_cleanup_push(EndBotThread, NULL); 
    while(true) {
        if (pauseSearch || game->gameState == STARTING) {
            SleepMilliseconds(1);
            continue;
        }
        SearchIteration();
    }
    pthread_cleanup_pop(1);
    return NULL;
}

void RestartBotThread(TetrisGame *game) {
    pauseSearch = true;
    SleepMilliseconds(1.0f);

    BotState currentState = {0};
    currentState.combo = game->combo;
    currentState.b2b = game->backToBack;
    GenBitboard(game, &currentState);
    CarryPieces(game, &currentState);
    ResetSearch(&currentState);

    SleepMilliseconds(1.0f);
    pauseSearch = false;
}

void MakeBestMove(TetrisGame *game) {
    BotAction actions[BOT_ACTIONS_ARRAY_SIZE] = {0};
    GetBest(actions);
    bool harddrop = false;
    for (int i = 0; (i < BOT_ACTIONS_ARRAY_SIZE && !harddrop); i++) {
        switch (actions[i]) {
            case MOVE_HARDDROP:
                harddrop = true;
                Harddrop(game);
                break;
            case MOVE_RIGHT:
                MoveRight(game);
                break;
            case MOVE_LEFT:
                MoveLeft(game);
                break;
            case MOVE_CW:
                RotateClockwise(game);
                break;
            case MOVE_CCW:
                RotateCounterClockwise(game);
                break;
            case MOVE_SOFTDROP:
                while (MoveDown(game));
                break;
            case MOVE_HOLD:
                Hold(game);
                break;
            default:
                break;
        }
    }
    printf("\n");
    BotState currentState = {0};
    GenBitboard(game, &currentState);
    currentState.combo = game->combo;
    currentState.b2b = game->backToBack;
    CarryPieces(game, &currentState);
    ResetSearch(&currentState);
}
void EndBotThread(void *arg) {
    EndBot();
    printf("Exited Bot Thread successfully\n");
}

static void GenBitboard(TetrisGame *game, BotState *state) {
    for (int r = 0; r < BOT_ROWS; r++) {
        for (int c = 0; c < BOT_COLUMNS; c++) {
            if (game->board[r][c] != EMPTY) {
                state->board[c] |= (1 << r);
            }
        }
    }
}

static void CarryPieces(TetrisGame *game, BotState *state) {
    state->active = (BotPiece)game->active;
    state->held = (BotPiece)game->held;
    int bagIndex;
    for(int i = 0; i < BOT_QUEUE_LENGTH; i++) {
        bagIndex = game->next + i;
        if (bagIndex < TETRIS_SEVEN_BAG_SIZE) {
            state->queue[i] = game->currentBag[bagIndex];
        } else {
            state->queue[i] = game->nextBag[(bagIndex - TETRIS_SEVEN_BAG_SIZE)];
        }
    }
    if (state->active == BOT_EMPTY || game->gameState == WAITING) {
        state->active = state->queue[0];
        state->next++;
    }
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

static int SleepMilliseconds(float milliseconds) {
    int seconds = 0;
    if (milliseconds >= 1000.0f) {
        seconds = (int)(milliseconds / 1000);
        milliseconds -= (seconds * 1000.0f);
    }
    struct timespec sleep = {seconds, (int)(milliseconds * 1000000.0f)};
    return nanosleep(&sleep, NULL);
}
