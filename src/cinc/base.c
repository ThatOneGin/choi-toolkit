#include "aux.h"
#include "state.h"
#include "strio.h"
#include "types.h"
#include <stdio.h>

#define fns_len 2

void writestr(struct cinc_state *cs) {
  struct object *toprint = pop_top(cs);
  switch (toprint->kind) {
    case Number:
      printf("%d\n", toprint->value.as_int);
      break;
    case Null:
      printf("null\n");
      break;
    case String:
      printf("%s\n", toprint->value.as_name.data);
      break;
    case Function:
      printf("%p", toprint->value.as_fn);
      break;
    default:
    // name or other kinds should be illegal in here.
    // also, the parser wouldn't let this happen.
    break;
  }
}

void cinc_get_type(struct cinc_state *cs) {
  struct object *toprint = pop_top(cs);
  switch (toprint->kind) {
    case Number:
      push_string(cs, "Number");
      break;
    case Null:
      push_string(cs, "null");
      break;
    case String:
      push_string(cs, "String");
      break;
    case Function:
      push_string(cs, "Function");
      break;
    default:
      push_string(cs, "Unkwnown");
      break;
  }
}

void registry_base(struct cinc_state *cs) {
  struct Symbol fn[] = {
    {.name = tostrv("write"), .value.as_fn = (regF)writestr, .type = Function},
    {.name = tostrv("type"), .value.as_fn = (regF)cinc_get_type, .type = Function}
  };
  
  for (int i = 0; i < fns_len; i++) {
    registryEnv(cs, fn[i]);
  }
}