#ifndef MEM_H
#define MEM_H
#include <stdlib.h>
#include "state.h"

#define arena_initsize 256

typedef struct Choi_Arena {
  int can_be_freed;
  void *memory;
  struct Choi_Arena *next;
} Choi_Arena;

/*
** internal memory management
** each block have a fixed lifetime
*/

void cinc_init_arena();
void *cinc_alloc_from_arena(size_t size);
void cinc_arena_realloc(struct cinc_state *cs, Choi_Arena *a, size_t sz);
void cinc_free_arena();

/*
** interface for arenas
*/

void *cinc_realloc(struct cinc_state *cs, void *ptr, unsigned long size);
void cinc_free(void *ptr);
void *cinc_buffer(unsigned long size);
void *cinc_vector(unsigned long size, size_t size_of);
#define cinc_malloc(size) cinc_realloc(NULL, size);
#endif
