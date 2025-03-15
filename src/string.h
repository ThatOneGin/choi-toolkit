#ifndef choi_str_h
#define choi_str_h
#include <stdio.h>
#include "choivm.h"
#include "arena.h"

#define LABEL_TABLE_CAPACITY 100
#define UNKNOWN_OP_TABLE_CAP 100

typedef struct {
  char *data;
  size_t size;
} Str;

Str str_capture(Str s, char delim);
Str to_str(char *data) ;
int str_cmp(Str a, Str b);
Str str_split(Str *s, char delim);
int str_atoi(Str s);
Str str_triml(Str s);
Str str_trimr(Str s);
Str str_trimlr(Str s);
Str str_trims(Str str);
Str str_cpy(Str s);
#endif
