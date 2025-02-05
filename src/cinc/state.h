#ifndef STATE_H
#define STATE_H
#include "strv.h"
#include "types.h"

#define ini_obj_size 10

struct Symbol {
  strv name;
  cinc_type type;
  union Value value;
};

struct cinc_state {
  struct object *stack;
  size_t sp;
  size_t stack_capacity;

  struct Symbol *ENV;
  size_t ep;
  size_t env_capacity;

  // is the state ok? or should exit?
  int ok;
};

struct cinc_state open_state();
void close_state(int code, struct cinc_state cs);
struct object search_by_name(struct cinc_state cs, char *name);
struct object *peek(struct cinc_state *cs, int pos);
struct object *gettop(struct cinc_state *cs);
int can_insert(struct cinc_state *cs);
int can_remove(struct cinc_state *cs);
void registryEnv(struct cinc_state *cs, struct Symbol sym);
#endif
