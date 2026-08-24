#include <raylib.h>
#include <stdio.h>
#include "input.h"

typedef struct {
    float pps;
    float timer;
} BotInterval;

static BotInterval botInterval = (BotInterval){3.0f, 0.0f};

static void IncrementBotTimer(TetrisGame *bot, float frametime);

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
                StopBot();
                StartBot(bot);
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

    if (bot->gameState == RUNNING) {
        IncrementBotTimer(bot, frametime);
    }
}

static void IncrementBotTimer(TetrisGame *bot, float frametime) {
    if (botInterval.timer < 0) {
        botInterval.timer = frametime;
        MakeBotMove(bot);
    }
    botInterval.timer += frametime;
    if (botInterval.timer >= (1000.0f / botInterval.pps)) {
        botInterval.timer = -12345;
        RequestBotMove(bot);
    }
}