#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"
#include "choivm.h"
#include "io.h"
#include "string.h"

gsb_vm gsb_vm_init(Arena arena) {
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

void dump_registers(gsb_vm vm) {
  printf("[\n");
  for (int i = 0; i < r_count; i++) {
    printf("  register R%d: [%lu]\n", i+1, vm.registers[i]);
  }
  printf("]\n");
}

void dump_stack(gsb_vm vm) {
  printf("stack: [ ");
  for (size_t i = 0; i < vm.sp; i++) {
    printf("%lu ", vm.stack[i]);
  }
  printf("]\n");
}

void set_registers_to_zero(gsb_vm *vm) {
  for (int i = 0; i < r_count; i++) {
    vm->registers[i] = (value) 0;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: choivm <input.choivm>\nERROR: not enough arguments.\n");
    exit(1);
  }

  Arena arena = init_arena(2048);
  gsb_vm vm = gsb_vm_init(arena);
  set_registers_to_zero(&vm);
  vm.halt = 0;

  choi_header ch = readprogram(&vm, argv[1]);

  while (vm.ip < ch.program_size && !vm.halt) {
    exec(&vm, vm.program[vm.ip]);
  }

  if (argc == 3 && strcmp(argv[2], "--dump-registers") == 0) {
    dump_registers(vm);
  }

  if (argc == 3 && strcmp(argv[2], "--dump-stack") == 0) {
    dump_stack(vm);
  }

  free(arena.mem);
  free(vm.program);
  return 0;
}
