#ifndef TYPES_H
#define TYPES_H
#include "strv.h"

typedef void (*regF)(struct cinc_state *);

typedef enum { Number, Name, Null, String, Function } cinc_type;

union Value {
  int as_int;
  strv as_name;
  int as_null;
  regF as_fn;
};

struct object {
  cinc_type kind;
  union Value value;
};
#endif
