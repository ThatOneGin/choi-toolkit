#include "choivm.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>

char *opcode_as_str(int opcode) {
  switch (opcode) {
    case OP_PUSH:
      return "OP_PUSH";
    case OP_ADD:
      return "OP_ADD";
    case OP_SUB:
      return "OP_SUB";
    case OP_MOV:
      return "OP_MOV";
    case OP_DUMP:
      return "OP_DUMP";
    case OP_DROP:
      return "OP_DROP";
    case OP_JMP:
      return "OP_JMP";
    case OP_WRITE:
      return "OP_WRITE";
    case OP_PUSH_ADDR:
      return "OP_PUSH_ADDR";
    case OP_CALL:
      return "OP_CALL";
    case OP_SYSCALL:
      return "OP_SYSCALL";
    case OP_RET:
      return "OP_RET";
    case OP_POP_STACK:
      return "OP_POP_STACK";
    case OP_PUSH_STACK:
      return "OP_PUSH_STACK";
    case OP_HALT:
      return "OP_HALT";
    case OP_CMP:
      return "OP_CMP";
    case OP_JNZ:
      return "OP_JNZ";
    case OP_JZ:
      return "OP_JZ";
    default:
      return "(nil)";
  }
}

void disasm(instruction *program, size_t len) {
  for (size_t i = 0; i < len; i++) {
    instruction c = program[i];
    printf("%lu  ", i);
    switch (c.opcode) {
    case OP_PUSH:
      printf("opcode: %s, operand: %lu, arg1: %d\n", opcode_as_str(c.opcode), c.operand, c.arg1);
      break;
    case OP_ADD:
      printf("opcode: %s, arg1: %d, arg2: %d\n", opcode_as_str(c.opcode), c.arg1, c.arg2);
      break;
    case OP_SUB:
      printf("opcode: %s, arg1: %d, arg2: %d\n", opcode_as_str(c.opcode), c.arg1, c.arg2);
      break;
    case OP_MOV:
      printf("opcode: %s, arg1: %d, arg2: %d\n", opcode_as_str(c.opcode), c.arg1, c.arg2);
      break;
    case OP_DUMP:
      printf("opcode: %s, arg1: %d\n", opcode_as_str(c.opcode), c.arg1);
      break;
    case OP_DROP:
      printf("opcode: %s, arg1: %d\n", opcode_as_str(c.opcode), c.arg1);
      break;
    case OP_JMP:
      printf("opcode: %s, operand: %lu\n", opcode_as_str(c.opcode), c.operand);
      break;
    case OP_WRITE:
      printf("opcode: %s, arg1: %d\n", opcode_as_str(c.opcode), c.arg1);
      break;
    case OP_PUSH_ADDR:
      printf("opcode: %s, operand %lu, arg1: %d\n", opcode_as_str(c.opcode), c.operand, c.arg1);
      break;
    case OP_CALL:
      printf("opcode: %s, arg1: %d, arg2: %d\n", opcode_as_str(c.opcode), c.arg1, c.arg2);
      break;
    case OP_SYSCALL:
      printf("opcode: %s\n", opcode_as_str(c.opcode));
      break;
    case OP_RET:
      printf("opcode: %s\n", opcode_as_str(c.opcode));
      break;
    case OP_POP_STACK:
      printf("opcode: %s, arg1: %d\n", opcode_as_str(c.opcode), c.arg1);
      break;
    case OP_PUSH_STACK:
      printf("opcode: %s, operand: %lu\n", opcode_as_str(c.opcode), c.operand);
      break;
    case OP_HALT:
      printf("opcode: %s\n", opcode_as_str(c.opcode));
      break;
    case OP_CMP:
      printf("opcode: %s, arg1: %d, arg2: %d\n", opcode_as_str(c.opcode), c.arg1, c.arg2);
      break;
    case OP_JNZ:
      printf("opcode: %s, operand: %lu\n", opcode_as_str(c.opcode), c.operand);
      break;
    case OP_JZ:
      printf("opcode: %s, operand: %lu\n", opcode_as_str(c.opcode), c.operand);
      break;
    default:
      printf("; Not Recognized ip: %lu\n", i);
      break;
    }
  }
}

void print_file_info(choi_header ch) {
  printf("version: %d\n", ch.version);
  printf("program size: %lu\n", ch.program_size);
  printf("magic: %X\n", ch.magic);
  printf("entry point (main procedure address): %d\n", ch.entry);
  printf("------------------------------------------------------\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: choi_disasm <input.choivm>\nERROR: not enough arguments.\n");
    exit(1);
  }
  gsb_vm vm = {0};
  choi_header ch = readprogram(&vm, argv[1]);

  print_file_info(ch);
  disasm(vm.program, ch.program_size);
  
  free(vm.program);
  return 0;
}
