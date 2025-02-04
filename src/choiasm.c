#include <stdio.h>
#include "choivm.h"
#include "io.h"
#include "string.c"
#include "io.c"
#include "arena.h"
#include "arena.c"

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: choiasm <input.casm>\nERROR: not enough arguments.\n");
    exit(1);
  }

  Str src = openfile(argv[1]);
  Arena strbuffer = init_arena(102);

  label_table lb = {0};

  choi_header ch = {
    .magic = magic_number,
    .version = choi_version,
  };
  choi_asm ca = {
    .lb = lb,
    .program = {0},
    .memory = {0},
    .memory_capacity = vm_memory_capacity,
    .program_capacity = 1000,
  };

  size_t psize = parse_file(src, &ca, &ch, &strbuffer);
  ch.program_size = psize;
  ch.memory_size = ca.memory_size;

  writefile("a.choivm", ca.program, ca, ch);

  free(src.data);
  free(strbuffer.mem);
  return 0;
}
