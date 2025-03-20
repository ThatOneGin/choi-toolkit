#ifndef gsb_h
#define gsb_h
#include <stdint.h>
#include <stdio.h>
#include "arena.h"

#define maxloop 1000
#define max_program_size 1000
#define choi_version 01
#define magic_number 255
#define vm_capacity 2 * 1000
#define ret_stack_cap 100

enum Registers {
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,
  r8,
  r_count
};
/**
typedef union {
  int as_int;
  void *as_ptr;
} value;
*/

typedef uintptr_t value;

typedef struct {
  int magic;
  int version;
  int entry;
  size_t program_size;
  size_t size;
}choi_header;

typedef struct {
  enum {
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOV,
    OP_DUMP,
    OP_DROP,
    OP_JMP,
    OP_CALL,
    OP_RET,
    OP_WRITE,
    OP_PUSH_ADDR,
    OP_SYSCALL,
    OP_POP_STACK,
    OP_PUSH_STACK,
    OP_HALT,
    OP_CMP,
    OP_JZ,
    OP_JNZ,
    OP_INC,
  }opcode;
  
  value operand;
  int arg1, arg2;
}instruction;

typedef struct {
  int halt;
  value registers[r_count];

  int *ret_stack;
  size_t ret_sp;
  
  uintptr_t *stack;
  size_t sp;

  size_t ip;
  instruction *program;

  unsigned char memory[vm_capacity];
  size_t size;
}gsb_vm;

gsb_vm gsb_vm_init(Arena arena);
void push_vm(gsb_vm *vm, uintptr_t value);
uintptr_t pop_vm(gsb_vm *vm);
void ret_push_vm(gsb_vm *vm, int value);
int ret_pop_vm(gsb_vm *vm);
void exec(gsb_vm *vm, instruction i);
void dump_registers(gsb_vm vm);
void dump_stack(gsb_vm vm);
void set_registers_to_zero(gsb_vm *vm);
void vm_write(gsb_vm *vm, int len);
#endif
