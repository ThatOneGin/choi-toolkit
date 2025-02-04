#ifndef CINC_H
#define CINC_H
#include <stdio.h>
#include <stdarg.h>
#include "mem.h"


#define cinc_close(code) exit((code))
#define cinc_isnt(arg1, arg2) ((arg1) != (arg2))
#define cinc_is(arg1, arg2) ((arg1) == (arg2))

#define cinc_error(msg) fprintf(stderr, "[cinc] Error: %s\n", msg)
#define cinc_warn(msg) (fprintf(stderr, "[cinc] Warning: %s\n", msg))
#endif
