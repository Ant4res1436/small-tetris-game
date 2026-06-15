#define _POSIX_C_SOURCE 199309L
#include "botcontrol.h"
#include "bot/bot.h"
#include <time.h>
#include <stdio.h>
#include <pthread.h>

static BotNodes nodes = {0};

static void EndBotThread(void *arg);
static void GenBitboard(TetrisGame *game, uint32_t *bitboard);
static void GenQueue(TetrisGame *game, BotPiece *queue);
static void PrintBoard(uint32_t *board);
static int SleepMilliseconds(float milliseconds);

void* StartBotThread(void *arg) {
    TetrisGame *game = (TetrisGame*)arg;
    BotState gameState = {0};
        

    GenBitboard(game, gameState.board);
    gameState.active = game->active;
    gameState.held = game->held;
    GenQueue(game, gameState.queue);
    StartBot(&gameState, &nodes);
    pthread_cleanup_push(EndBotThread, NULL);


    while(true) {
        SearchIteration(&nodes);
        SleepMilliseconds(10000.0f);
    }
    pthread_cleanup_pop(1);
    return NULL;
}

void MakeBestMove(void) {
    BotAction actions[BOT_ACTIONS_ARRAY_SIZE] = {0};
    GetBest(actions, &nodes);
}
static void EndBotThread(void *arg) {
    EndBot();
    printf("Exited Bot Thread successfully\n");
}

static void GenBitboard(TetrisGame *game, uint32_t *bitboard) {
    for (int r = 0; r < BOT_ROWS; r++) {
        for (int c = 0; c < BOT_COLUMNS; c++) {
            if (game->board[r][c] != EMPTY) {
                bitboard[c] |= (1 << r);
            }
        }
    }
}

static void GenQueue(TetrisGame *game, BotPiece *queue) {
    int bagIndex;
    for(int i = 0; i < BOT_QUEUE_LENGTH; i++) {
        bagIndex = game->next + i;
        if (bagIndex < TETRIS_SEVEN_BAG_SIZE) {
            queue[i] = game->currentBag[bagIndex];
        } else {
            queue[i] = game->nextBag[(bagIndex - TETRIS_SEVEN_BAG_SIZE)];
        }
        
    }
}

void PrintBoard(uint32_t *board) {
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
