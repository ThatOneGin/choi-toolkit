/*
** Auxiliary library
** includes stack manipulation.
** and stack checks.
*/

#include "aux.h"
#include "cinc.h"
#include "state.h"
#include "strio.h"
#include "types.h"
#include <stdio.h>

/*
*** Stack manipulation
*/

void push_obj(struct cinc_state *cs, struct object obj) {
  if (!can_insert(cs)) {
    cinc_error("Stack overflow.");
    cinc_close(1);
  }
  cs->stack[cs->sp++] = obj;
}

void push_string(struct cinc_state *cs, char *str) {
  if (!can_insert(cs)) {
    cinc_error("Stack overflow.");
    cinc_close(1);
  }
  cs->stack[cs->sp++] =
      (struct object){.kind = String, .value = {.as_name = tostrv(str)}};
}

void push_int(struct cinc_state *cs, int num) {
  if (!can_insert(cs)) {
    cinc_error("Stack overflow.");
    cinc_close(1);
  }
  cs->stack[cs->sp++] =
      (struct object){.kind = Number, .value = {.as_int = num}};
}

struct object *pop_top(struct cinc_state *cs) {
  if (cs->sp > 0) {
    struct object *obj = gettop(cs);
    cs->sp--;
    return obj;
  }

  return NULL;
}

/*
** Check functions:
**  check if the top of the stack is of a certain type
**  if it is, return it.
**  else, close and exit.
*/

int check_int(struct cinc_state *cs) {
  struct object *top = gettop(cs);
  if (cinc_isnt(top->kind, Number)) {
    cinc_error("The top of the stack is not a number.");
    cinc_close(1);
  }

  cs->sp--;

  return top->value.as_int;
}

char *check_string(struct cinc_state *cs) {
  struct object *top = gettop(cs);

  if (cinc_isnt(top->kind, String)) {
    printf("Found type: %d\n", top->kind);
    cinc_error("The top of the stack is not a string.");
    cinc_close(1);
  }

  cs->sp--;

  return top->value.as_name.data;
}