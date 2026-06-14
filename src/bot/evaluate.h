#ifndef EVALUATE_H
#define EVALUATE_H

#include "bot.h"

typedef struct {
    int32_t height;
    int32_t upperHalf;
    int32_t upperQuarter;
    int32_t holes;
    int32_t holeCover;
    int32_t change; 
    int32_t terrain;

    int32_t single;
    int32_t double_; // double is reserved lol
    int32_t triple;
    int32_t tetris;
    int32_t tss;
    int32_t tsd;
    int32_t tst;
    int32_t combo;
    int32_t b2b;

} EvalScoring;

void SetDefaultScoring();
void Evaluate(BotState *state);

#endif