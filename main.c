#include <raylib.h>
#include <raymath.h>
#include <pthread.h>
#include "src/botcontrol.h"
#include "src/tetris.h"
#include "src/gui.h"
#include "src/input.h"
#include "src/bot/bot.h"

typedef struct {
    TetrisGame *game;
    float pps;
    float timer;
} BotInterval;

static BotInterval botInterval = {0};

static void IncrementBotTimer(float frametime);

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Tetris");
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(FPS);
    GuiLoadTextures();
    srand(time(NULL));
    uint32_t seed = rand();
    TetrisGame player = TetrisGameNew(seed);

    TetrisGame bot = TetrisGameNew(seed);
    pthread_t botThread;
    pthread_create(&botThread, NULL, StartBotThread, &bot);

    botInterval = (BotInterval){&bot, 10.0f, 0.0f};
    float frametime;

    while (!WindowShouldClose())
    {
        frametime = (GetFrameTime() * 1000.0f);
        if (bot.gameState == RUNNING) {
            IncrementBotTimer(frametime);
        }
        UpdateTetrisGame(&player, frametime);
        UpdateTetrisGame(&bot, frametime);
        HandleInput(&player, &bot, &botThread);
        BeginDrawing();
        ClearBackground(BG_COLOR);
        DrawTetrisGames(&player, &bot);
        EndDrawing();
        SyncGames(&player, &bot);
    }
    GuiUnloadTextures();
    CloseWindow();

    pthread_cancel(botThread);
    pthread_join(botThread, NULL);
    
    return 0;
}

static void IncrementBotTimer(float frametime) {
    botInterval.timer += frametime;
    if (botInterval.timer >= (1000.0f / botInterval.pps)) {
        botInterval.timer = 0;
        MakeBestMove(botInterval.game);
    }
}