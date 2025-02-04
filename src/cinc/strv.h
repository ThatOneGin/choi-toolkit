#ifndef STRV_H
#define STRV_H
#include <stdio.h>

typedef char *cstr;

typedef struct {
  char *data;
  size_t size;
} strv;
#endif