/*
** Interpreter state
*/

#include "cinc.h"
#include "mem.h"
#include "state.h"
#include "strio.h"
#include "types.h"
#include <stdio.h>

struct cinc_state open_state() {
  struct cinc_state c = {0};
  c.ENV = cinc_vector(ini_obj_size, sizeof(struct Symbol));
  c.stack = cinc_vector(ini_obj_size, sizeof(struct object));
  c.stack_capacity = ini_obj_size;
  c.env_capacity = ini_obj_size;

  return c;
}

/*
** Symbol table functions
*/

struct object search_by_name(struct cinc_state cs, char *name) {
  strv name_as_strv = tostrv(name);

  for (size_t i = 0; i < cs.ep; i++) {
    if (strvcmp(cs.ENV[i].name, name_as_strv)) {
      return (struct object){.value = cs.ENV[i].value, .kind = cs.ENV[i].type};
    }
  }

  return (struct object){.value = {.as_null = 0}, .kind = Null};
}

void registryEnv(struct cinc_state *cs, struct Symbol sym) {
  if (cs->ep >= cs->env_capacity) {
    cinc_error("ENV overflow.");
    cinc_close(1);
  }

  cs->ENV[cs->ep++] = sym;
}

/*
** Stack functions
*/

struct object *peek(struct cinc_state *cs, int pos) {
  if (pos < 0) {
    return NULL;
  }
  return &cs->stack[pos];
}

struct object *gettop(struct cinc_state *cs) { return &cs->stack[cs->sp - 1]; }

int can_insert(struct cinc_state *cs) {
  return cs->sp < cs->stack_capacity ? 1 : 0;
}

int can_remove(struct cinc_state *cs) {
  return cs->sp > 0 ? 1 : 0;
}