#include <raylib.h>
#include <stdio.h>
#include "input.h"

void HandleInput(TetrisGame *player, TetrisGame *bot, pthread_t *botThread) {
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
                EndBotThread(NULL);
                pthread_cancel(*botThread);
                pthread_join(*botThread, NULL);
                pthread_create(botThread, NULL, StartBotThread, player);
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
    //printf("Active: %d, Next: %d, Held: %d\n",instance->pieceInfo->active ,instance->currentBag[instance->next], instance->held);
}