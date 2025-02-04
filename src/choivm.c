#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"
#include "io.c"
#include "choivm.h"
#include "io.h"
#include "io.h"
#include "string.h"
#include "arena.c"
#include "syscall.c"
#include "syscall.h"

#define maxloop 1000

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

void vm_write(gsb_vm *vm, int len) {
  int addr = pop_vm(vm);
  fprintf(stdout, "%.*s\n", len, vm->memory+addr);
}

void push_vm(gsb_vm *vm, int value) {
  if (vm->sp >= 256) {
    printf("ERROR: Ret Stack overflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->stack[vm->sp] = value;
  vm->sp++;
}

int pop_vm(gsb_vm *vm) {
  if (vm->sp <= 0) {
    printf("\033[31mERROR\033[0m: Stack underflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->sp--;
  int val = vm->stack[vm->sp];
  return val;
}

void ret_push_vm(gsb_vm *vm, int value) {
  if (vm->ret_sp >= 100) {
    printf("ERROR: Ret Stack overflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->ret_stack[vm->ret_sp] = value;
  vm->ret_sp++;
}

int ret_pop_vm(gsb_vm *vm) {
  if (vm->ret_sp <= 0) {
    printf("\033[31mERROR\033[0m: Stack underflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->ret_sp--;
  return vm->stack[vm->ret_sp];
}

void exec(gsb_vm *vm, instruction i) {
  // protection to infinite loops
  static int iter = 0;

  if (iter >= maxloop) {
    vm->halt = 1;
  }

  switch (i.opcode) {
  case OP_PUSH:
    vm->registers[i.arg1] = (value) {.as_int = i.operand.as_int};
		vm->ip += 1;
    break;
  case OP_ADD:
    vm->registers[i.arg1] = (value) {.as_int = vm->registers[i.arg1].as_int + vm->registers[i.arg2].as_int};
		vm->ip += 1;
    break;
  case OP_SUB:
    vm->registers[i.arg1] = (value) {.as_int = vm->registers[i.arg1].as_int - vm->registers[i.arg2].as_int};
		vm->ip += 1;
    break;
  case OP_MOV:
    vm->registers[i.arg1] = vm->registers[i.arg2];
		vm->ip += 1;
    break;
  case OP_DUMP:
    printf("%d\n", vm->registers[i.arg1].as_int);
		vm->ip += 1;
    break;
  case OP_DROP:
    vm->registers[i.arg1] = (value) {.as_int = 0};
		vm->ip += 1;
    break;
  case OP_JMP:
    vm->ip = i.operand.as_int;
    iter++;
    break;
  case OP_CALL:
    ret_push_vm(vm, i.arg2);
    vm->ip = i.arg1;
    break;
  case OP_RET:
    vm->ip = ret_pop_vm(vm);
    break;
  case OP_WRITE:
    vm_write(vm, vm->registers[i.arg1].as_int);
		vm->ip += 1;
    break;
  case OP_PUSH_ADDR:
    push_vm(vm, i.operand.as_int);
		vm->ip += 1;
    break;
  case OP_SYSCALL:
    vm->registers[7].as_int = check_for_syscall_kind(vm);
		vm->ip += 1;
    break;
  case OP_POP_STACK:
    vm->registers[i.arg1].as_int = pop_vm(vm);
		vm->ip += 1;
    break;
  case OP_PUSH_STACK:
    if (i.arg1 == 0) {
      push_vm(vm, i.operand.as_int);
    } else {
      push_vm(vm, vm->registers[i.operand.as_int].as_int);
    }
		vm->ip += 1;
    break;
  case OP_HALT:
    vm->halt = 1;
    break;
  case OP_CMP:
    push_vm(vm, vm->registers[i.arg1].as_int - vm->registers[i.arg2].as_int);
    vm->ip += 1;
    break;
  case OP_JZ:
    if (pop_vm(vm) == 0) {
      vm->ip = i.operand.as_int;
    } else {
      vm->ip += 1;
    }
    iter++;
    break;
  case OP_JNZ:
    if (pop_vm(vm) != 0) {
      vm->ip = i.operand.as_int;
    } else {
      vm->ip += 1;
    }
    iter++;
    break;
  case OP_INC:
    vm->registers[i.arg1].as_int++;
    vm->ip += 1;
    break;
  default:
    printf("Instruction opcode not recognized. %d", i.opcode);
    exit(1);
  }
}

void dump_registers(gsb_vm vm) {
  printf("[\n");
  for (int i = 0; i < r_count; i++) {
    printf("  register R%d: [as_int: %d, as_ptr: %p]\n", i+1, vm.registers[i].as_int, vm.registers[i].as_ptr);
  }
  printf("]\n");
}

void dump_stack(gsb_vm vm) {
  printf("stack: [ ");
  for (size_t i = 0; i < vm.sp; i++) {
    printf("%d ", vm.stack[i]);
  }
  printf("]\n");
}


void set_registers_to_zero(gsb_vm *vm) {
  for (int i = 0; i < r_count; i++) {
    vm->registers[i] = (value) {.as_int = 0};
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
