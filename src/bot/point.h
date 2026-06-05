#ifndef POINT_H
#define POINT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int32_t x;
    int32_t y;
} BotPoint;

static void PointAdd(BotPoint *point, BotPoint add);
static void PointSubtract(BotPoint *point, BotPoint subtract);
static bool PointEquals(BotPoint *left, BotPoint* right);

#endif