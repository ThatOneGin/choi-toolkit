#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "exec.h"
#include "choivm.h"
#include "syscall.h"

void vm_write(gsb_vm *vm, int len) {
  int addr = pop_vm(vm);
  fprintf(stdout, "%.*s\n", len, vm->memory+addr);
}

void push_vm(gsb_vm *vm, uintptr_t value) {
  if (vm->sp >= 256) {
    printf("ERROR: Ret Stack overflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->stack[vm->sp] = value;
  vm->sp++;
}

uintptr_t pop_vm(gsb_vm *vm) {
  if (vm->sp <= 0) {
    printf("\033[31mERROR\033[0m: Stack underflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->sp--;
  int val = vm->stack[vm->sp];
  return val;
}

void ret_push_vm(gsb_vm *vm, int value) {
  if (vm->ret_sp >= ret_stack_cap) {
    printf("ERROR: Ret Stack overflow at ip: %lu\n", vm->ip);
    exit(1);
  }
  vm->ret_stack[vm->ret_sp++] = value;
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
    vm->registers[i.arg1] = (value) i.operand;
		vm->ip += 1;
    break;
  case OP_ADD:
    vm->registers[i.arg1] = (value) vm->registers[i.arg1] + vm->registers[i.arg2];
		vm->ip += 1;
    break;
  case OP_DIV:
    vm->registers[i.arg1] = (value) vm->registers[i.arg1] / vm->registers[i.arg2];
		vm->ip += 1;
    break;
  case OP_MUL:
    vm->registers[i.arg1] = (value) vm->registers[i.arg1] * vm->registers[i.arg2];
		vm->ip += 1;
    break;
  case OP_SUB:
    vm->registers[i.arg1] = (value) vm->registers[i.arg1] - vm->registers[i.arg2];
		vm->ip += 1;
    break;
  case OP_MOV:
    vm->registers[i.arg1] = vm->registers[i.arg2];
		vm->ip += 1;
    break;
  case OP_DUMP:
    printf("%lu\n", vm->registers[i.arg1]);
		vm->ip += 1;
    break;
  case OP_DROP:
    vm->registers[i.arg1] = (value) 0;
		vm->ip += 1;
    break;
  case OP_JMP:
    vm->ip = i.operand;
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
    vm_write(vm, vm->registers[i.arg1]);
		vm->ip += 1;
    break;
  case OP_PUSH_ADDR:
    push_vm(vm, i.operand);
		vm->ip += 1;
    break;
  case OP_SYSCALL:
    vm->registers[7] = check_for_syscall_kind(vm);
		vm->ip += 1;
    break;
  case OP_POP_STACK:
    vm->registers[i.arg1] = pop_vm(vm);
		vm->ip += 1;
    break;
  case OP_PUSH_STACK:
    if (i.arg1 == 0) {
      push_vm(vm, i.operand);
    } else {
      push_vm(vm, vm->registers[i.operand]);
    }
		vm->ip += 1;
    break;
  case OP_HALT:
    vm->halt = 1;
    break;
  case OP_CMP:
    push_vm(vm, vm->registers[i.arg1] - vm->registers[i.arg2]);
    vm->ip += 1;
    break;
  case OP_JZ:
    if (pop_vm(vm) == 0) {
      vm->ip = i.operand;
    } else {
      vm->ip += 1;
    }
    iter++;
    break;
  case OP_JNZ:
    if (pop_vm(vm) != 0) {
      vm->ip = i.operand;
    } else {
      vm->ip += 1;
    }
    iter++;
    break;
  case OP_INC:
    vm->registers[i.arg1]++;
    vm->ip += 1;
    break;
  default:
    printf("Instruction opcode not recognized. %d", i.opcode);
    exit(1);
  }
}
