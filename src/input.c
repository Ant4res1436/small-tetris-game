#include <raylib.h>
#include <stdio.h>
#include "input.h"
#include <pthread.h>

//#define ENTER_BOT_CONTROL

static pthread_t botThread;

typedef struct {
    TetrisGame *game;
    float pps;
    float timer;
} BotInterval;

static BotInterval botInterval = {0};

static void IncrementBotTimer(float frametime);

void SetUpBot(TetrisGame *bot) {
    pthread_create(&botThread, NULL, StartBotThread, bot);
    botInterval = (BotInterval){bot, 2.0f, 0.0f};
}

void StopBotThread(void) {
    pthread_cancel(botThread);
    pthread_join(botThread, NULL);
}

void HandleInput(TetrisGame *player, TetrisGame *bot, float frametime) {
    KeyboardKey key = (KeyboardKey)GetKeyPressed();
    while (key != 0)  {
        switch (key) {
            case KEY_RIGHT:
                ToggleDasRight(player);
                break;
            case KEY_LEFT:
                ToggleDasLeft(player);
                break;
            case KEY_UP: 
                RotateClockwise(player);
                break;
            case KEY_LEFT_CONTROL: 
                RotateCounterClockwise(player);
                break;
            case KEY_LEFT_SHIFT: 
                Hold(player);
                break;
            case KEY_DOWN: 
                ToggleSoftdrop(player);
                break;
            case KEY_SPACE:
                Harddrop(player);
                break;
            case KEY_F4:
                uint32_t seed = rand();
                *player = TetrisGameNew(seed);
                *bot = TetrisGameNew(seed);
                // Reset Bot Thread
                botInterval.timer = 0;
                RestartBotThread(bot);
                break;
            default: 
                break;
        }    
        key = (KeyboardKey)GetKeyPressed();
    }
    if (IsKeyReleased(KEY_DOWN)) {
        ToggleSoftdrop(player);
    }
    if (player->right.enabled && IsKeyUp(KEY_RIGHT)) {
        ToggleDasRight(player);
    }
    if (player->left.enabled && IsKeyUp(KEY_LEFT)) {
        ToggleDasLeft(player);
    }

    #ifdef ENTER_BOT_CONTROL
        if (IsKeyPressed(KEY_ENTER)) {
            MakeBestMove(botInterval.game);
        }
    #else
        if (bot->gameState == RUNNING) {
            IncrementBotTimer(frametime);
        }
    #endif
    //printf("Active: %d, Next: %d, Held: %d\n",instance->pieceInfo->active ,instance->currentBag[instance->next], instance->held);
}

static void IncrementBotTimer(float frametime) {
    botInterval.timer += frametime;
    if (botInterval.timer >= (1000.0f / botInterval.pps)) {
        botInterval.timer = 0;
        MakeBestMove(botInterval.game);
    }
}