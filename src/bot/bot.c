#include "bot.h"

static Arena arena = (Arena){0};

void AllocateMemory(size_t size) {
    arena = ArenaCreate(size);
}

void FreeMemory() {
    ArenaDestroy(&arena);
    arena = (Arena){0};
}