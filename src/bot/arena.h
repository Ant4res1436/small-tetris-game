#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

#define ARENA_KiB(amount) (size_t)((amount) * 1024)
#define ARENA_MiB(amount) (size_t)(ARENA_KiB(amount) * 1024)
#define ARENA_GiB(amount) (size_t)(ARENA_MiB(amount) * 1024)

typedef struct {
    void *memory;
    size_t size;
    size_t position;
} Arena;

Arena ArenaCreate(size_t size);
void *ArenaAlloc(Arena *arena, size_t size);
void ArenaReset(Arena *arena);
void ArenaZero(Arena *arena);
void ArenaDestroy(Arena *arena);

#endif