#include "arena.h"
#include <string.h>
#include <stdio.h>

#define ARENA_ALIGN(n) (((n) + ((sizeof(void*)) - 1)) & ~((sizeof(void*)) - 1))

Arena ArenaCreate(size_t size) {
    Arena arena = {0};
    arena.memory = malloc(size);
    if (arena.memory == NULL) {
        free(arena.memory);
        printf("ERROR: ARENA: Failed to create Arena (%zu bytes)\n", size);
        exit(EXIT_FAILURE);
    }
    arena.size = size;
    ArenaZero(&arena);
    return arena;
}
void *ArenaAlloc(Arena *arena, size_t size) {
    size_t alignedSize = ARENA_ALIGN(size);
    if (arena->position + alignedSize > arena->size) {
        return NULL;
    }
    void *ptr = (arena->memory + arena->position);
    arena->position += alignedSize;
    return ptr;
}

void ArenaReset(Arena *arena) {
    arena->position = 0;
}
void ArenaZero(Arena *arena) {
    arena->position = 0;
    memset(arena->memory, 0, arena->size);
}
void ArenaDestroy(Arena *arena) {
    free(arena->memory);
}