#include "evaluate.h"
#include <stdio.h>

static EvalScoring scoring = {0};
static float wellPreference[BOT_COLUMNS] = {20, -30, 40, 50, 40, 40, 50, 40, -30, 20};

void SetDefaultScoring() {
    scoring.terrainChange = -30.0f;
    scoring.holes = -5.0f;
    scoring.holeCover = -0.5f;
    scoring.height = -10.0f;
    scoring.upperHalf = -33.3f;
    scoring.upperQuarter = -100.0f;

    scoring.single = -100.0f;
    scoring.double_ = -40.0f;
    scoring.triple = -15.0f;
    scoring.tetris = 100.0f;
    scoring.tss = 80.0f;
    scoring.tsd = 130.0f;
    scoring.tst = 200.0f;
    scoring.combo = 10.0f;
    scoring.b2b = 20.0f;
}

void Evaluate(BotState *state) {
    state->score = 0;
    uint8_t maxHeight = 0;
    uint8_t minHeight = 0;
    uint8_t heights[BOT_COLUMNS] = {0};
    for (int c = 0; c < BOT_COLUMNS; c++) {
        heights[c] = (31 - __builtin_clz(state->board[c]));
        if (heights[c] > heights[maxHeight]) {
            maxHeight = c;
        }
        if (heights[c] < heights[minHeight]) {
            minHeight = c;
        }
    }
    
    for (int c = 0; c < BOT_COLUMNS; c++) {
        for (int r = 0; r < heights[c]; r++) {
            if((state->board[c] & (1 << r)) == 0) {
                state->score += scoring.holes;
            }
        }
        state->score += scoring.terrainChange * abs(heights[c] - heights[(c - 1)]);
        if (abs((heights[c] - heights[(c - 1)]) > 2)) {
            state->score += scoring.terrainChange * heights[c];
        }
        if (heights[c] != __builtin_popcount(state->board[c])) {
            //state->score += scoring.terrainChange;
        }
    }
    state->score += maxHeight * scoring.height;
    state->score += 100 * state->lines;
    //state->score += scoring.combo * (state->combo + 1);
}