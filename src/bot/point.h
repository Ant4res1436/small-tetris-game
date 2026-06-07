#ifndef POINT_H
#define POINT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int8_t x;
    int8_t y;
} BotPoint;

void PointAdd(BotPoint *point, BotPoint add);
void PointSubtract(BotPoint *point, BotPoint subtract);
bool PointEquals(BotPoint *left, BotPoint* right);

#endif