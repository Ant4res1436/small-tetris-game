// evaluate.c
#include "evaluate.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    float holes;
    float topHoleCover;
    float change;
    float maxHeight;
    float upperHalf;
    float upperQuarter;
    float wellDepth;

    float one;
    float two;
    float three;
    float tetris;
    float tss;
    float tsd;
    float tst;
    float combo;
    float b2b;
    float pc;
} Weights;

static Weights weights;

void SetDefaultScoring(void) {
    weights.holes = -200;
    weights.topHoleCover = -30;
    weights.change = -50;
    weights.maxHeight = -30;
    weights.upperHalf = -40;
    weights.upperQuarter = -120;
    weights.wellDepth = 3;

    weights.one = -150;
    weights.two = -70;
    weights.three = -35;
    weights.tetris = 400;
    weights.tss = 150;
    weights.tsd = 450;
    weights.tst = 600;
    weights.combo = 20;
    weights.b2b = 40;
    weights.pc = 999;
}

void Evaluate(BotState *state) {
    state->score = 0;

    int32_t temp;
    int32_t indexMaxHeight = 0;
    int32_t indexMinHeight = BOT_ROWS;
    int32_t heights[BOT_COLUMNS] = {0};
    int32_t holes = 0;

    for (int c = 0; c < BOT_COLUMNS; c++) {
        heights[c] = 31 - __builtin_clz(state->board[c]);
        if (heights[c] > indexMaxHeight) {
            indexMaxHeight = c;
        }
        if (heights[c] < indexMinHeight) {
            indexMinHeight = c;
        }
        holes += heights[c] - __builtin_popcount(state->board[c]);
    }
    state->score += weights.maxHeight * heights[indexMaxHeight];
    if (heights[indexMaxHeight] >= (BOT_ROWS * 0.5f)) {
        if (heights[indexMaxHeight] >= (BOT_ROWS * 0.75f)) {
            state->score += weights.upperQuarter;
        } else {
            state->score += weights.upperHalf;
        }
    }
    state->score += weights.holes * holes;

    int32_t topHole = 0;
    int32_t topHoleCover = 0;
    for (int c = 0; c < BOT_COLUMNS; c++) {
        for (int r = (heights[c] - 1); r >= 0; r--) {
            if ((state->board[c] & (1 << r)) != 1) {
                if (r > topHole) {
                    topHole = r;
                    topHoleCover = (heights[c] - r);
                } else if (r == topHole) {
                    topHoleCover += (heights[c] - r);
                }
                break;
            }
        }
    }
    state->score += weights.topHoleCover * topHoleCover;


    int32_t change = 0;
    for (int c = 0; c < (BOT_COLUMNS - 1); c++) {
        if (c != indexMinHeight) {
            temp = abs((heights[c] - heights[c + 1]));
            if (temp > 2) {
                change += temp * temp;
            }
        }
    }
    state->score += weights.change * change;



    uint32_t wellDepth;
    if (indexMinHeight == 0 ||
        heights[(indexMinHeight + 1) < heights[(indexMinHeight - 1)]]) {
        wellDepth = abs((heights[indexMinHeight] - heights[indexMinHeight + 1])); 
    } else if (indexMinHeight == (BOT_COLUMNS - 1) ||
               heights[(indexMinHeight - 1) < heights[(indexMinHeight + 1)]]) {
        wellDepth = abs((heights[indexMinHeight] - heights[indexMinHeight - 1])); 
    }
    state->score += weights.wellDepth * wellDepth;


    bool b2b = false;
    switch (state->lines) {
        case 1:
            state->score += weights.one;
            b2b = false;
            break;
        case 2:
            state->score += weights.two;
            b2b = false;
            break;
        case 3:
            state->score += weights.three;
            b2b = false;
            break;
        case 4:
            state->score += weights.tetris;
            b2b = true;
            break;
        default:
            b2b = state->b2b;
            break;
    }
    if (b2b) {
        state->score += weights.b2b;
    }
    if (state->combo >= 0) {
        state->score += weights.combo;
    }
    if (state->board[indexMaxHeight] == 0) {
        state->score += weights.pc;
    }
}

