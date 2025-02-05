#include "aux.c"
#include "gen.h"
#include "lex.c"
#include "lex.h"
#include "mem.c"
#include "mem.h"
#include "parser.c"
#include "parser.h"
#include "state.c"
#include "state.h"
#include "strio.c"
#include "strio.h"
#include "base.c"
#include "gen.c"
#include "../io.c"
#include "../arena.c"
#include "../exec.c"
#include "../syscall.c"

gsb_vm vm_init(Arena arena) {
  gsb_vm vm;
  vm.ip = 0;

  vm.stack = alloc(&arena, 1024);
  vm.sp = 0;
  
  vm.ret_stack = alloc(&arena, 100);
  vm.ret_sp = 0;

  if (vm.ret_stack == NULL) {
    free(arena.mem);
    exit(1);
  }
  return vm;
}

int main(int argc, char **argv) {
  atexit(cinc_free_arena);
  cinc_init_arena();
  
  Arena vm_arena = init_arena(2048);
  gsb_vm vm = vm_init(vm_arena);

  struct cinc_state cs = open_state();
  registry_base(&cs);

  strv src = file2strv(argv[1]);
  cinc_lexer_state cls = {.source = src, .pos = 0, .current = src.data[0]};

  cinc_parser_state p = {.lex = &cls, .current = cinc_lex(&cls), .cs = cs};

  choi_asm ca = {0};
  choi_header ch = {0};

  ch.magic = 0xFF;
  ch.entry = 0;

  while (at(&p).type != EFO) {
    insert_to_program(&ch, &ca, parse_stmt(&p));
  }

  ch.program_size = ca.program_size;
  ch.version = 01;

  for (size_t i = 0; i < ca.program_size; i++) {
    exec(&vm, ca.program[i]);
  }

  free(vm_arena.mem);
  return 0;
}