#include "botcontrol.h"
#include "bot/bot.h"
#include <unistd.h>
#include <stdio.h>

static bool MakeBotMove(TetrisGame *game);
static uint32_t *GenBitboard(TetrisGame *game);
static BotPiece *GenQueue(TetrisGame *game);
static int SleepMilliseconds(float milliseconds);

void* StartBotThread(void* arg) {
    BotArgs *botArgs = (BotArgs*)arg;
    SleepMilliseconds(-botArgs->game->elapsed * 1000.0f);
    while (botArgs->game->gameState == STARTING) {
        SleepMilliseconds(0.1f);
    }
    int32_t offset = -ToGroundOffset(botArgs->game);
    if (offset > 0) {
        ToggleSoftdrop(botArgs->game);
        SleepMilliseconds(botArgs->game->gravityInterval * (offset + 1));
        ToggleSoftdrop(botArgs->game);
    }
    for(int i = 0; true; i++) {
        printf("Bot Thread doing things %d\n", i);
        SleepMilliseconds((botArgs->piecesPerSecond * 1000.0f) + 0.1f);
    }
    return NULL;
}

bool MakeBotMove(TetrisGame *game) {
    uint32_t *board = GenBitboard(game);
    BotPiece *queue = GenQueue(game);

    BotActions *moves = GetMoves(
        board,
        (BotPiece)game->active,
        (BotPiece)game->held,
        queue,
        game->backToBack,
        game->combo
    );

    for (int m = 0; moves[m] != MOVE_HARDDROP; m++) {
        switch (moves[m]) {
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
    }
    Harddrop(game);
    if (game->gameState == WAITING) {
        usleep(((int)TETRIS_LINE_CLEAR_DELAY + 1) * 1000);
    }
}

static uint32_t *GenBitboard(TetrisGame *game) {
    return NULL;
}
static BotPiece *GenQueue(TetrisGame *game) {
    return NULL;
}

static int SleepMilliseconds(float milliseconds) {
    return usleep((int)(milliseconds * 1000.0f));
}
