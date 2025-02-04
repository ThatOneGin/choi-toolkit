#ifndef STRING_H
#define STRING_H
#include "state.h"
#include "stdio.h"

strv tostrv(cstr str);
int strvcmp(strv s1, strv s2);
strv file2strv(const char *file);
int strv_atoi(cstr num);
#endif
