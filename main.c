#include <raylib.h>
#include <raymath.h>
#include <pthread.h>
#include "src/tetris.h"
#include "src/gui.h"
#include "src/input.h"

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Tetris");
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(FPS);
    GuiLoadTextures();
    srand(time(NULL));
    uint32_t seed = rand();
    TetrisGame player = TetrisGameNew(seed);

    TetrisGame bot = TetrisGameNew(seed);
    StartBot(&bot);
    
    float frametime;
    while (!WindowShouldClose())
    {
        frametime = (GetFrameTime() * 1000.0f);

        UpdateTetrisGame(&player, frametime);
        UpdateTetrisGame(&bot, frametime);


        HandleInput(&player, &bot, frametime);
        BeginDrawing();
        ClearBackground(BG_COLOR);
        DrawTetrisGames(&player, &bot);
        EndDrawing();
        if (SyncGames(&player, &bot) == 1) {
            UpdateBot(&bot);
        }
    }
    GuiUnloadTextures();
    CloseWindow();

    StopBot();
    
    return 0;
}

