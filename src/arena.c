#include "arena.h"
#include <stdio.h>

Arena init_arena(size_t size) {
  Arena a;
  a.mem = malloc(size);
  a.offset = 0;
  a.size = size;

  if (a.mem == NULL) {
    perror("Couldn't allocate memory");
    exit(1);
  }
  return a;
}

void arena_realloc(Arena *arena) {
  arena->mem = realloc(arena->mem, arena->size * 2);
  arena->size *= 2;
  if (arena->mem == NULL) {
    perror("Couldn't reallocate memory");
    exit(1);
  }
}

void *alloc(Arena *arena, size_t size) {
  if (arena->offset + size <= arena->size) {
    void *block = (char *) arena->mem + size;
    arena->offset += size;
    return block;
  } else {
    arena_realloc(arena);
    return alloc(arena, size);
  }
}
