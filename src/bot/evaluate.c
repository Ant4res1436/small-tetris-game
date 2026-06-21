// evaluate.c
#include "evaluate.h"
#include <stdlib.h>

typedef struct {
    float holes;
    float holeDepth;       // penalize holes buried deeper, more costly to dig out
    float aggregateHeight;
    float bumpiness;       // ignores the well column
    float maxHeight;
    float wellDepth;       // reward a clean, consistent well
    float wellMismatch;    // punish well column drifting between placements
    float tetris;          // big reward for 4-line clears
    float partialClear;    // punish 1-3 line clears that aren't combo continuations
    float combo;           // reward sustaining a combo
    float comboBreak;      // punish breaking an active combo without a Tetris
} EvalWeights;

static EvalWeights weights;
static int wellColumn = -1;  // which column the bot has committed to as the well

void SetDefaultScoring(void) {
    weights.holes           = -80.0f;
    weights.holeDepth       = -10.0f;
    weights.aggregateHeight = -3.0f;
    weights.bumpiness       = -15.0f;
    weights.maxHeight       = -15.0f;
    weights.wellDepth       = 8.0f;
    weights.wellMismatch    = -40.0f;
    weights.tetris          = 400.0f;
    weights.partialClear    = -150.0f;
    weights.combo           = 60.0f;
    weights.comboBreak      = -120.0f;
}

static int ColumnHeight(uint32_t col) {
    if (col == 0) return 0;
    return 32 - __builtin_clz(col);
}

static int ColumnHoles(uint32_t col) {
    if (col == 0) return 0;
    int height = ColumnHeight(col);
    uint32_t filled = (1u << height) - 1;
    return __builtin_popcount(filled & ~col);
}

// sum of (depth below surface) for every hole in a column - deep holes cost more
static int ColumnHoleDepth(uint32_t col) {
    if (col == 0) return 0;
    int height = ColumnHeight(col);
    int depth = 0;
    for (int row = 0; row < height; row++) {
        if (!((col >> row) & 1)) {
            depth += (height - row);
        }
    }
    return depth;
}

// pick the column with the lowest height as the well, biased toward edges (0 or 9)
static int FindBestWellColumn(int heights[BOT_COLUMNS]) {
    int best = 0;
    for (int c = 1; c < BOT_COLUMNS; c++) {
        if (heights[c] < heights[best]) {
            best = c;
        }
        // prefer edge columns on ties since they're easier to keep clean
        if (heights[c] == heights[best] && (c == 0 || c == BOT_COLUMNS - 1)) {
            best = c;
        }
    }
    return best;
}

void Evaluate(BotState *state) {
    int heights[BOT_COLUMNS];
    int holes = 0;
    int holeDepth = 0;
    int aggregateHeight = 0;
    int maxHeight = 0;

    for (int c = 0; c < BOT_COLUMNS; c++) {
        heights[c] = ColumnHeight(state->board[c]);
        holes += ColumnHoles(state->board[c]);
        holeDepth += ColumnHoleDepth(state->board[c]);
        aggregateHeight += heights[c];
        if (heights[c] > maxHeight) maxHeight = heights[c];
    }

    int well = FindBestWellColumn(heights);

    // bumpiness ignoring the well column, since it's SUPPOSED to be lower
    int bumpiness = 0;
    for (int c = 0; c < BOT_COLUMNS - 1; c++) {
        if (c == well || c + 1 == well) continue;
        bumpiness += abs(heights[c] - heights[c + 1]);
    }

    // well depth relative to its immediate neighbor
    int neighbor = (well == 0) ? heights[1] : heights[well - 1];
    if (well != 0 && well != BOT_COLUMNS - 1) {
        int rightNeighbor = heights[well + 1];
        neighbor = (neighbor < rightNeighbor) ? neighbor : rightNeighbor;
    }
    int wellDepth = neighbor - heights[well];
    if (wellDepth < 0) wellDepth = 0;

    // punish switching which column is the well between placements
    int wellMismatch = 0;
    if (wellColumn != -1 && wellColumn != well) {
        wellMismatch = 1;
    }
    wellColumn = well;

    float score =
        (weights.holes * holes) +
        (weights.holeDepth * holeDepth) +
        (weights.aggregateHeight * aggregateHeight) +
        (weights.bumpiness * bumpiness) +
        (weights.maxHeight * maxHeight) +
        (weights.wellDepth * wellDepth) +
        (weights.wellMismatch * wellMismatch);

    // line clear handling: only Tetrises are "free", anything else should
    // ideally be part of an active combo chain
    if (state->lines == 4) {
        score += weights.tetris;
    } else if (state->lines > 0) {
        score += weights.partialClear;
        if (state->combo > 0) {
            // softens the partial-clear penalty if it's extending a combo
            score += (weights.combo * state->combo);
        }
    } else if (state->combo <= 0 && state->lines == 0) {
        // no clear, no combo - neutral, nothing to add or punish here
    }

    if (state->lines == 0 && state->combo > 0) {
        // this candidate would have broken an active combo by not clearing
        score += weights.comboBreak;
    }

    state->score = score;
}