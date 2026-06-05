#include "point.h"

void PointAdd(BotPoint *point, BotPoint add) {
    point->x += add.x;
    point->y += add.y;
}

void PointSubtract(BotPoint *point, BotPoint subtract) {
    point->x -= subtract.x;
    point->y -= subtract.y;
}

bool PointEquals(BotPoint *left, BotPoint* right) {
    return ((left->x == right->x) && (left->y == right->y));
}