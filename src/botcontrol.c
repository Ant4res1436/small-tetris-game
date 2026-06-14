#define _POSIX_C_SOURCE 199309L
#include "botcontrol.h"
#include "bot/bot.h"
#include "bot/evaluate.h"
#include <time.h>
#include <stdio.h>

static bool MakeBotMove(TetrisGame *game);
static void GenBitboard(TetrisGame *game, uint32_t *bitboard);
static void GenQueue(TetrisGame *game, BotPiece *queue);
static int SleepMilliseconds(float milliseconds);

void* StartBotThread(void* arg) {
    BotArgs *botArgs = (BotArgs*)arg;
    SetDefaultScoring();
    SleepMilliseconds(-botArgs->game->elapsed * 1000.0f);
    while (botArgs->game->gameState == STARTING) {
        SleepMilliseconds(0.1f);
    }
    
    for(int i = 0; true; i++) {
        printf("Bot Thread doing things %d\n", i);
        SleepMilliseconds(((1.0f / botArgs->piecesPerSecond) * 1000.0f));
        MakeBotMove(botArgs->game);
    }
    
    return NULL;
}

bool MakeBotMove(TetrisGame *game) {
    BotState currentState = {0};
    GenBitboard(game, currentState.board);
    GenQueue(game, currentState.queue);
    /*
    GetActions(
        actions,
        board,
        (BotPiece)game->active,
        (BotPiece)game->held,
        queue,
        game->backToBack,
        game->combo,
        1
    );
    
    if (game->gameState == WAITING) {
        SleepMilliseconds(TETRIS_LINE_CLEAR_DELAY + 1);
    }
    for (int m = 0; (m < BOT_ACTIONS_ARRAY_SIZE && actions[m] != MOVE_HARDDROP); m++) {
        switch (actions[m]) {
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
            case MOVE_HOLD:
                Hold(game);
                break;
            case MOVE_SOFTDROP: 
                int32_t offset = -ToGroundOffset(game);
                if (offset > 0) {
                    SleepMilliseconds(game->gravityInterval * offset);
                    while (MoveDown(game));
                }
                break;
            default:
                return false;
                break;
        }
        SleepMilliseconds(TETRIS_ARR);
    }
    Harddrop(game);
    printf("harddrop");
    */
    return false;
}

static void GenBitboard(TetrisGame *game, uint32_t *bitboard) {
    
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

static int SleepMilliseconds(float milliseconds) {
    int seconds = 0;
    if (milliseconds > 1000.0f) {
        seconds = (int)(milliseconds / 1000);
        milliseconds -= (seconds * 1000.0f);
    }
    struct timespec sleep = {seconds, (int)(milliseconds * 1000000.0f)};
    return nanosleep(&sleep, NULL);
}
