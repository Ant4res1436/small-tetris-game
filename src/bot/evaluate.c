// evaluate.c
#include "evaluate.h"
#include <stdlib.h>
#include <stdio.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define UPPER_HALF 10
#define UPPER_QUARTER 15
#define LIMIT_COVER(a) MIN(a, 6)

typedef struct {
    float holes;
    float holeCover;
    float heights;
    float change;
    float upperHalf;
    float upperQuarter;
    float wellDepth;

    float normal[5];
    float mini[3];
    float tspin[4];
    float b2b;
    float b2bClear;
    float combo;
    float pc;
} Weights;

static Weights weights = {0};

void SetDefaultScoring(void) {
    weights.holes = -1.5f;
    weights.holeCover = -0.2f;
    weights.heights = -0.4f;
    weights.change = -0.2f;
    weights.upperHalf = -1.5f;
    weights.upperQuarter = -5.0f;
    weights.wellDepth = 0.3f;

    weights.normal[0] = 0.0f;
    weights.normal[1] = -2.0f;
    weights.normal[2] = -1.5f;
    weights.normal[3] = -1.0f;
    weights.normal[4] = 3.5f;

    weights.b2b = 0.5f;
    weights.b2bClear = 1.0f;
    weights.combo = 1.5f;
    weights.pc = 15.0f;
}

void Evaluate(BotState *state) {
    state->score = 0;

    int heights[BOT_COLUMNS] = {0};
    int iMaxHeight = 0;
    int iMinHeight = 0;
    int holes = 0;
    int cover = 0;
    uint32_t coverMask;
    int trailing;
    
    for (int c = 0; c < BOT_COLUMNS; c++) {
        heights[c] = 32 - __builtin_clz(state->board[c]);
        if (heights[c] > heights[iMaxHeight]) {
            iMaxHeight = c;
        }
        if (heights[c] < heights[iMinHeight]) {
            iMinHeight = c;
        }
        holes += (heights[c] - __builtin_popcount(state->board[c]));

        coverMask = (state->board[c] ^ UINT32_MAX) & ((1 << heights[c]) - 1);
        while (coverMask != 0) {
            trailing = __builtin_ctz(coverMask);
            cover += LIMIT_COVER(heights[c] - trailing);
            coverMask &= (UINT32_MAX ^ (1 << trailing));
        }
    }

    uint32_t wellMask = UINT32_MAX;
    for (int c = 0; c < BOT_COLUMNS; c++) {
        if (c == iMinHeight) {
            continue;
        }
        wellMask &= state->board[c];
    }
    wellMask >>= heights[iMinHeight];

    int change = __builtin_popcount(INT32_MAX ^ state->board[0]);
    for (int c = 0; c < (BOT_COLUMNS - 1); c++) {
       change += __builtin_popcount(state->board[c] ^ state->board[(c + 1)]);
    }   
    change += __builtin_popcount(INT32_MAX ^ state->board[(BOT_ROWS - 1)]);

    state->score += weights.heights * heights[iMaxHeight];
    state->score += weights.holes * holes;
    state->score += weights.holeCover * cover;
    state->score += weights.wellDepth * __builtin_popcount(wellMask);
    state->score += weights.change * change;
    if (heights[iMaxHeight] > UPPER_HALF) {
        state->score += weights.upperHalf * (heights[iMaxHeight] - UPPER_HALF);
    }
    if (heights[iMaxHeight] > UPPER_QUARTER) {
        state->score += weights.upperHalf * (heights[iMaxHeight] - UPPER_QUARTER);
    }
    state->score += weights.normal[state->lines];
    if (state->highestCombo > 0) {
        state->score += weights.combo * state->highestCombo * state->highestCombo;
    }
    if (state->lines > 0) {
        if (state->lines == 4) {
            state->score += weights.b2bClear;
        } else {
            state->b2b = false;
        }
    }
    if (state->b2b) {
        state->score += weights.b2b;
    }
}

