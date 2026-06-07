#include <raylib.h>
#include <raymath.h>


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
    float frametime;

    while (!WindowShouldClose())
    {
        frametime = (GetFrameTime() * 1000.0f);
        UpdateTetrisGame(&player, frametime);
        UpdateTetrisGame(&bot, frametime);
        HandleInput(&player, &bot);
        BeginDrawing();
        ClearBackground(BG_COLOR);
        DrawTetrisGames(&player, &bot);
        EndDrawing();
        SyncGames(&player, &bot);
    }
    GuiUnloadTextures();
    CloseWindow();
    
    return 0;
}