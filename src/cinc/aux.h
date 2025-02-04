#ifndef CINC_AUX
#define CINC_AUX
#include "state.h"
#include "types.h"

void push_obj(struct cinc_state *cs, struct object obj);
void push_string(struct cinc_state *cs, char *str);
void push_int(struct cinc_state *cs, int num);
struct object *pop_top(struct cinc_state *cs);
int check_int(struct cinc_state *cs);
char *check_string(struct cinc_state *cs);
#endif