#ifndef EVALUATE_H
#define EVALUATE_H

#include "bot.h"

typedef struct {
    float terrainChange;
    float holes;
    float holeCover;
    float height;
    float upperHalf;
    float upperQuarter;

    float single;
    float double_; // double is reserved lol
    float triple;
    float tetris;
    float tss;
    float tsd;
    float tst;
    float combo;
    float b2b;
} EvalScoring;

void SetDefaultScoring();
void Evaluate(BotState *state);

#endif