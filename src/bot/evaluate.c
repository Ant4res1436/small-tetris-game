#include "evaluate.h"
#include <stdio.h>

#define COLUMN_BIT 0b0001000000000000

static EvalScoring defaultScoring = {0};

void SetDefaultScoring() {
    EvalScoring scoring = {0};
    scoring.height = -10;
    scoring.upperHalf = -50;
    scoring.upperQuarter = -125;
    scoring.holes = -2;
    scoring.holeCover = -3;
    scoring.change = -5;
    scoring.terrain = -17;

    scoring.single = -100;
    scoring.double_ = -50;
    scoring.triple = -25;
    scoring.tetris = 200;
    scoring.tss = 100;
    scoring.tsd = 250;
    scoring.tst = 400;
    scoring.combo = 10;
    scoring.b2b = 25;
    defaultScoring = scoring;
}

void Evaluate(BotMove *move) {
    move->score = 0;
    // Height Eval
    int32_t maxHeight = (BOT_ROWS - 1);
    for (int r = 0; r < BOT_ROWS; r++) {
        if (move->board[r] == BOT_EMPTY_ROW) {
            maxHeight == (r + 1);
        }
    }
    move->score += ((1 << maxHeight) * defaultScoring.height);

    int32_t heights[BOT_COLUMNS] = {0};
    int32_t holes = 0;
    for (int r = 0; r <= maxHeight; r++) {
        for (int c = 0; c < BOT_COLUMNS; c++) {
            if ((move->board[r] & (COLUMN_BIT >> c)) > 0) {
                if (heights[c] != r) {
                    move->score += defaultScoring.holeCover;
                }
                heights[c] = (r + 1);
            }
        }
    }


    int32_t minHeightIndex = move->board[maxHeight];
    float avgHeights = 0;
    for (int c = 0; c < BOT_COLUMNS; c++) {
        if (heights[c] < move->board[minHeightIndex]) {
            minHeightIndex = c;
        }
        avgHeights += heights[c];
    }
    avgHeights = (avgHeights - (move->board[minHeightIndex] * 10)) / BOT_COLUMNS;
    if (avgHeights >= 1) {
        move->score += (int32_t)(avgHeights * defaultScoring.terrain);
    }
    
        

    uint32_t heightChanges[(BOT_COLUMNS - 1)] = {0};
    for (int c = 0; c < (BOT_COLUMNS - 1); c++) {
        heightChanges[c] = abs((heights[c] - heights[c + 1]));
        if (!(c == minHeightIndex || (c + 1) == minHeightIndex) && heightChanges[c] > 2) {
            move->score += defaultScoring.change;
        }
    }

    // Line Scoring
    if (move->lines > 0) {
        bool tspin = move->twoCornerRule && move->threeCornerRule;
        switch (move->lines) {
            case 1:
                if (tspin) {
                    move->score += defaultScoring.tss;
                } else {
                    move->score += defaultScoring.single;
                }
                break;
            case 2:
                if (tspin) {
                    move->score += defaultScoring.tsd;
                } else {
                    move->score += defaultScoring.double_;
                }
                break;
            case 3:
                if (tspin) {
                    move->score += defaultScoring.tst;
                } else {
                    move->score += defaultScoring.triple;
                }
                break;
            case 4:            
                move->score += defaultScoring.tetris;
                break;
            default:
                break;
        }
        move->b2b = (tspin || move->lines == 4);
        if (move->b2b) {
            move->score += defaultScoring.b2b;
        } else {
            move->score -= defaultScoring.b2b;
        }
        move->score += (move->combo * defaultScoring.combo);
    }
}