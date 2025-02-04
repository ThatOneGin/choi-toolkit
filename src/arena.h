#ifndef ARENA_H
#define ARENA_H
#include <stdlib.h>
typedef struct {
  void *mem;
  size_t offset;
  size_t size;
}Arena;

Arena init_arena(size_t size);
void arena_realloc(Arena *arena);
void *alloc(Arena *arena, size_t size);
#endif
