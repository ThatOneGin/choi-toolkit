#include "gen.h"
#include "cinc.h"
#include <stdio.h>

#define inst(opc, opr, a1, a2) ((instruction){.opcode = (opc), .operand = {opr}, .arg1 = a1, .arg2 = a2})

void insert_to_program(choi_header *ch, choi_asm *ca, cinc_expr node) {
  switch (node.kind) {
    case NUM_LITERAL:
      ca->program[ca->program_size++] = 
      inst(OP_PUSH_STACK, node.value.as_num_literal, 0, 0);
      break;
    case BINOP:
      insert_to_program(ch, ca, node.value.as_binary_op->lhs);
      insert_to_program(ch, ca, node.value.as_binary_op->rhs);

      ca->program[ca->program_size++] =
      inst(OP_POP_STACK, 0, 0, 0);
      ca->program[ca->program_size++] =
      inst(OP_POP_STACK, 0, 1, 0);

      switch (node.value.as_binary_op->op) {
        default:
        case '+':
          ca->program[ca->program_size++] =
          inst(OP_ADD, 0, 0, 1);
          ca->program[ca->program_size++] =
          inst(OP_PUSH_STACK, 0, 1, 0);
          break;
        case '-':
          ca->program[ca->program_size++] =
          inst(OP_SUB, 0, 0, 1);
          ca->program[ca->program_size++] =
          inst(OP_PUSH_STACK, 0, 1, 0);
          break;
      }
      break;
    default:
      printf("kind: %d\n", node.kind);
      cinc_error("Operation not allowed.");
      cinc_close(1);
      break;
  }
}