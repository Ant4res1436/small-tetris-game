#include <raylib.h>
#include <stdio.h>
#include "input.h"

void HandleInput(TetrisGame *left, TetrisGame *right) {
    KeyboardKey key = (KeyboardKey)GetKeyPressed();
    while (key != 0)  {
        switch (key) {
            case KEY_RIGHT:
                ToggleDasRight(left);
                break;
            case KEY_LEFT:
                ToggleDasLeft(left);
                break;
            case KEY_UP: 
                RotateClockwise(left);
                break;
            case KEY_LEFT_CONTROL: 
                RotateCounterClockwise(left);
                break;
            case KEY_LEFT_SHIFT: 
                Hold(left);
                break;
            case KEY_DOWN: 
                ToggleSoftdrop(left);
                break;
            case KEY_SPACE:
                Harddrop(left);
                break;
            case KEY_F4:
                uint32_t seed = rand();
                *left = TetrisGameNew(seed);
                *right = TetrisGameNew(seed);
                break;
            default: 
                break;
        }    
        key = (KeyboardKey)GetKeyPressed();
    }
    if (IsKeyReleased(KEY_DOWN)) {
        ToggleSoftdrop(left);
    }
    if (left->right.enabled && IsKeyUp(KEY_RIGHT)) {
        ToggleDasRight(left);
    }
    if (left->left.enabled && IsKeyUp(KEY_LEFT)) {
        ToggleDasLeft(left);
    }
    //printf("Active: %d, Next: %d, Held: %d\n",instance->pieceInfo->active ,instance->currentBag[instance->next], instance->held);
}