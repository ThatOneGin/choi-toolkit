#include "mem.h"
#include "cinc.h"
#include "state.h"
#include <stdint.h>

Choi_Arena *arena_list = {0};

void cinc_init_arena() {
  arena_list = malloc(sizeof(Choi_Arena));
  if (arena_list == NULL) {
    // Handle malloc failure
    exit(1);
  }
  arena_list->next = NULL;
  arena_list->can_be_freed = 1;
}

void *cinc_alloc_from_arena(size_t size) {
  if (arena_list == NULL || !arena_list) {
    cinc_init_arena();
  }

  Choi_Arena *current = arena_list;
  while (current->next != NULL) {
    current = current->next;
  }

  current->next = calloc(1, sizeof(Choi_Arena));

  if (current->next == NULL) {
    cinc_free_arena();
    exit(1);
  }

  current->next->memory = malloc(size);

  if (current->next->memory == NULL) {
    cinc_free_arena();
    exit(1);
  }

  return current->next->memory;
}

void cinc_free_arena() {
  if (arena_list == NULL || arena_list->can_be_freed == 0) {
    return;
  }

  Choi_Arena *current = arena_list;
  while (current != NULL) {
    Choi_Arena *next = current->next;
    if (current == arena_list) {
      free(current);
      current = next;
      continue;
    }
    free(current->memory);
    free(current);
    current = next;
  }

  arena_list = NULL;
}

void *cinc_realloc(struct cinc_state *cs, void *ptr, unsigned long size) {
  void *block;
  block = ptr ? realloc(ptr, size) : malloc(size);
  if (block == NULL) {
    cinc_error("Couldn't allocate memory block.");
    cinc_close(1);
  }
  return block;
}

void *cinc_vector(unsigned long size, size_t size_of) {
  void *block = cinc_alloc_from_arena(size * size_of);
  return block;
}

void *cinc_grow_vector(void *ptr, unsigned long size) {
  return cinc_realloc(NULL, ptr, size);
}

void *cinc_buffer(unsigned long size) {
  return cinc_vector(size, sizeof(char));
}