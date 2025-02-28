#ifndef choi_io_H
#define choi_io_H
#include "choivm.h"
#include "string.h"
#include <stdio.h>

Str openfile(char *filepath);
choi_header readprogram(gsb_vm *vm, char *filename);
void writefile(const char *filename, instruction *program, choi_asm ca, choi_header ch);
#endif
