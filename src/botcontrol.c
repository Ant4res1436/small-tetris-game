#define _POSIX_C_SOURCE 199309L
#include "botcontrol.h"
#include "bot/bot.h"
#include <time.h>
#include <stdio.h>
#include <pthread.h>

static BotNodes nodes = {0};

static void GenBitboard(TetrisGame *game, BotState *state);
static void CarryPieces(TetrisGame *game, BotState *state);
static void PrintBoard(uint32_t *board);
static int SleepMilliseconds(float milliseconds);

void* StartBotThread(void *arg) {
    TetrisGame *game = (TetrisGame*)arg;
    BotState gameState = {0};
        

    GenBitboard(game, &gameState);
    CarryPieces(game, &gameState);
    StartBot(&gameState, &nodes);
    pthread_cleanup_push(EndBotThread, NULL); 
    while(true) {
        SearchIteration(&nodes);
        printf("%d\n", nodes.count);
        break;
    }
    pthread_cleanup_pop(1);
    return NULL;
}

void MakeBestMove(TetrisGame *game) {
    BotAction actions[BOT_ACTIONS_ARRAY_SIZE] = {0};
    BotState gameState = {0};
    GenBitboard(game, &gameState);
    CarryPieces(game, &gameState);
    GetBest(actions, &gameState);
    bool harddrop = false;
    printf("Moves: ");
    for (int i = 0; (i < BOT_ACTIONS_ARRAY_SIZE && !harddrop); i++) {
        printf("%d->", actions[i]);
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
    printf("Active %d\n", state->active);
    printf("Held %d\n", state->held);
    int bagIndex;
    printf("queue: ");
    for(int i = 0; i < BOT_QUEUE_LENGTH; i++) {
        bagIndex = game->next + i;
        if (bagIndex < TETRIS_SEVEN_BAG_SIZE) {
            state->queue[i] = game->currentBag[bagIndex];
        } else {
            state->queue[i] = game->nextBag[(bagIndex - TETRIS_SEVEN_BAG_SIZE)];
        }
        printf("%d", state->queue[i]);
    }
    printf("\n");
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
