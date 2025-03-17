#include "parser.h"
#include "string.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int has_label(label_table *lb_t, Str name) {
  for (size_t i = 0; i < lb_t->size; i++) {
    if (str_cmp(lb_t->labels[i].name, name)) {
      return (int) 1;
    }
  }

  return 0;
}

int get_label_addr(label_table *lb_t, Str name) {
  for (size_t i = 0; i < lb_t->size; i++) {
    if (str_cmp(lb_t->labels[i].name, name)) {
      return lb_t->labels[i].addr;
    }
  }

  return -1;
}

void push_label(label_table *lb_t, label l) {
  for (int i = 0; (size_t) i < lb_t->size; i++) {
    if (str_cmp(lb_t->labels[i].name, l.name)) {
      printf("Found an existing label also named with %s, please rename.\n", l.name.data);
      exit(1);
    }
  }
  lb_t->labels[lb_t->size] = l;
  lb_t->size++;
}

void push_addr(label_table *lb_t, unknown_operand op) {
  if (lb_t->uo_size >= UNKNOWN_OP_TABLE_CAP) {
    printf("Array overflow, too many unknown operands.");
  }
  lb_t->unknown_operands[lb_t->uo_size++] = op;
}

unknown_operand pop_addr(label_table *lb_t) {
  lb_t->uo_size--;
  unknown_operand op = lb_t->unknown_operands[lb_t->uo_size];
  return op;
}

int parse_register(Str str) {
  Str reg = str_split(&str, ' ');

  if (str_cmp(reg, to_str("R1"))) {
    return r1;
  } else if (str_cmp(reg, to_str("R2"))) {
    return r2;
  } else if (str_cmp(reg, to_str("R3"))) {
    return r3;
  }  else if (str_cmp(reg, to_str("R4"))) {
    return r4;
  } else if (str_cmp(reg, to_str("R5"))) {
    return r5;
  } else if (str_cmp(reg, to_str("R6"))) {
    return r6;
  } else if (str_cmp(reg, to_str("R7"))) {
    return r7;
  }  else {
    return r8;
  }
}

Str parse_str(Str ln, Arena *arena) {
  if (*ln.data != '"') {
    printf("%.*s is not a '\"'\n", (int)ln.size, ln.data);
    exit(1);
  }
  ln.data += 1;
  ln.size -= 1;

  size_t i = 0;
  while (ln.data[i] != '"') {
    i++;
  }

  char *buffer = alloc(arena, (sizeof(char) * i) + 1);
  memcpy(buffer, ln.data, i);
  buffer[i] = '\0';

  return to_str(buffer);
}

instruction parse_line(Str ln, label_table *lb, size_t ip) {
  ln = str_triml(ln);
  Str i_n = str_split(&ln, ' ');

  if (str_cmp(i_n, to_str("push"))) {
    ln = str_triml(ln);
    int reg = parse_register(str_trimlr(ln));
    int op = str_atoi(str_trims(ln));

    return (instruction){.opcode = OP_PUSH, .operand = op, .arg1 = reg, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("add"))) {
    ln = str_triml(ln);
    int reg1 = parse_register(str_trimlr(ln));
    int reg2 = parse_register(str_trims(ln));

    return (instruction){.opcode = OP_ADD, .operand = 0,.arg1 = reg1, .arg2 = reg2};
  } else if (str_cmp(i_n, to_str("sub"))) {
    ln = str_triml(ln);
    int reg1 = parse_register(str_trimlr(ln));
    int reg2 = parse_register(str_trims(ln));

    return (instruction){.opcode = OP_SUB, .operand = 0,.arg1 = reg1, .arg2 = reg2};
  } else if (str_cmp(i_n, to_str("mul"))) {
    ln = str_triml(ln);
    int reg1 = parse_register(str_trimlr(ln));
    int reg2 = parse_register(str_trims(ln));

    return (instruction){.opcode = OP_MUL, .operand = 0,.arg1 = reg1, .arg2 = reg2};
  } else if (str_cmp(i_n, to_str("div"))) {
    ln = str_triml(ln);
    int reg1 = parse_register(str_trimlr(ln));
    int reg2 = parse_register(str_trims(ln));

    return (instruction){.opcode = OP_DIV, .operand = 0,.arg1 = reg1, .arg2 = reg2};
  } else if (str_cmp(i_n, to_str("dump"))) {
    ln = str_triml(ln);
    int reg = parse_register(str_triml(ln));
    return (instruction){.opcode = OP_DUMP, .operand = 0, .arg1 = reg, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("mov"))) {
    ln = str_triml(ln);
    int reg1 = parse_register(str_trimlr(ln));
    int reg2 = parse_register(str_trims(ln));
    
    return (instruction){.opcode = OP_MOV, .operand = 0, .arg1 = reg1, .arg2 = reg2};
  } else if (str_cmp(i_n, to_str("drop"))) {
    ln = str_triml(ln);
    int reg = parse_register(str_triml(ln));

    return (instruction){ .opcode = OP_DROP, .operand = 0, .arg1 = reg, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("jmp"))) {
    ln = str_triml(ln);
    Str name = str_trimlr(ln);
    int to_jmp = get_label_addr(lb, name);

    if (to_jmp < 0) {
      push_addr(lb, (unknown_operand){.addr = ip, .name = name});
    }
    
    return (instruction){.opcode = OP_JMP, .operand = to_jmp, .arg1 = 0, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("jz"))) {
    ln = str_triml(ln);
    Str name = str_trimlr(ln);

    int to_jmp = get_label_addr(lb, name);

    if (to_jmp < 0) {
      push_addr(lb, (unknown_operand){.addr = ip, .name = name});
    }
    
    return (instruction){.opcode = OP_JZ, .operand = to_jmp, .arg1 = 0, .arg2 = 0};
  }  else if (str_cmp(i_n, to_str("jnz"))) {
    ln = str_triml(ln);
    Str name = str_trimlr(ln);
    int to_jmp = get_label_addr(lb, name);

    if (to_jmp < 0) {
      push_addr(lb, (unknown_operand){.addr = ip, .name = name});
    }
    
    return (instruction){.opcode = OP_JNZ, .operand = to_jmp, .arg1 = 0, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("call"))) {
    ln = str_triml(ln);
    Str name = str_trimlr(ln);

    int arg1 = has_label(lb, name);
    
    if (arg1 < 0) {
      push_addr(lb, (unknown_operand){.addr = ip, .name = name});
    }
    
    return (instruction){.opcode = OP_CALL, .operand = 0, .arg1 = arg1, .arg2 = ip};
  } else if (str_cmp(i_n, to_str("ret"))) {
    ln = str_triml(ln);
    return (instruction){.opcode = OP_RET, .operand = 0, .arg1 = 0, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("write"))) {
    ln = str_triml(ln);
    int reg = parse_register(str_triml(ln));
    return (instruction){.opcode = OP_WRITE, .operand = 0, .arg1 = reg, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("syscall"))) {
    ln = str_triml(ln);
    return (instruction){.opcode = OP_SYSCALL, .operand = 0, .arg1 = 0, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("halt"))) {
    ln = str_triml(ln);
    return (instruction){.opcode = OP_HALT, .operand = 0, .arg1 = 0, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("cmp"))) {
    ln = str_triml(ln);

    int reg1 = parse_register(str_triml(ln));
    int reg2 = parse_register(str_trims(ln));

    return (instruction){.opcode = OP_CMP, .operand = 0, .arg1 = reg1, .arg2 = reg2};
  } else if (str_cmp(i_n, to_str("inc"))) {
    ln = str_triml(ln);

    int reg1 = parse_register(str_triml(ln));

    return (instruction){.opcode = OP_INC, .operand = 0, .arg1 = reg1, .arg2 = 0};
  } else {
    printf("Unknown instruction '%.*s'\n", (int)i_n.size, i_n.data);
    exit(1);
  }
}

void push_str_to_memory(choi_asm *ca, Str str) {
  assert(ca->size + str.size <= ca->capacity);
  memcpy(ca->memory + ca->size, str.data, str.size);

  ca->size += str.size;
}

void parse_special_operands(Str ln, choi_asm *ca, size_t ip, Arena *strArena) {
  ln = str_triml(ln);
  Str i_n = str_split(&ln, ' ');

  if (str_cmp(i_n, to_str("@proc"))) {
    ln = str_triml(ln);
    Str name = str_trimlr(ln);

    push_label(&ca->lb, (label) {
      .addr = ip,
      .name = name
    });
  } else if (str_cmp(i_n, to_str("@mem"))) {
    ln = str_triml(ln);
    int reg = parse_register(str_trimlr(ln));
    if (*str_trims(ln).data == '"') {
      Str str = parse_str(str_trims(ln), strArena);

      ca->program[ca->program_size++] = (instruction){.opcode = OP_PUSH_ADDR, .operand = ca->size, .arg1 = reg, .arg2 = 0};
      push_str_to_memory(ca, str);
      ca->program[ca->program_size++] = (instruction){.opcode = OP_PUSH, .operand = str.size, .arg1 = reg, .arg2 = 0};
    } else {
      int integer = str_atoi(str_trims(ln));
      ca->program[ca->program_size++] = (instruction){.opcode = OP_PUSH_ADDR, .operand = ca->size, .arg1 = reg, .arg2 = 0};
      memcpy(ca->memory, &integer, 1);
      ca->size += 1;
    }
  } else if (str_cmp(i_n, to_str("@pop"))) {
    ln = str_triml(ln);
    int reg = parse_register(str_trimlr(ln));

    ca->program[ca->program_size++] = (instruction){.opcode = OP_POP_STACK, .operand = 0, .arg1 = reg, .arg2 = 0};
  } else if (str_cmp(i_n, to_str("@push"))) {
    ln = str_triml(ln);
    int op;

    if (*str_triml(ln).data != 'R') {
      op = str_atoi(str_triml(ln));
      ca->program[ca->program_size++] = (instruction){.opcode = OP_PUSH_STACK, .operand = op, .arg1 = 0, .arg2 = 0};
    } else {
      op = parse_register(str_trimlr(ln));
      ca->program[ca->program_size++] = (instruction){.opcode = OP_PUSH_STACK, .operand = op, .arg1 = 1, .arg2 = 0};
    }
  } else {
    printf("Special operand not recognized '%.*s'\n", (int)i_n.size, i_n.data);
    exit(1);
  }
}

size_t parse_file(Str source, choi_asm *ca, choi_header *ch, Arena *strArena) {
  while (source.size > 0) {
    assert(ca->program_size < ca->program_capacity);
    Str line = str_triml(str_split(&source, '\n'));

    if (line.size > 0 && *line.data != ';') {
      if (*line.data == '@') {
        parse_special_operands(line, ca, ca->program_size, strArena);
      } else {
        ca->program[ca->program_size] = parse_line(line, &ca->lb, ca->program_size);
        ca->program_size++;
      }
    }
  }

  int main_label = get_label_addr(&ca->lb, to_str("main"));
  
  if (main_label < 0) {
    printf("Undefined reference to 'main' procedure.\n");
    exit(1);
  }

  while (ca->lb.uo_size > 0) {
    unknown_operand uo = pop_addr(&ca->lb);

    int label_addr = get_label_addr(&ca->lb, uo.name);

    if (label_addr < 0) {
      printf("Implicit declaration of label '%.*s'\n", (int) uo.name.size, uo.name.data);
      exit(1);
    }

    if (ca->program[uo.addr].opcode == OP_CALL) {
      ca->program[uo.addr].arg1 = label_addr;
    } else {
      ca->program[uo.addr].operand = label_addr;
    }
  }
  
  ch->entry = main_label;

  return ca->program_size;
}
