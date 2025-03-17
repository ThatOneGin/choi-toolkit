#ifndef PARSER_H
#define PARSER_H
#include "choivm.h"
#include "string.h"

typedef struct {
  int addr;
  Str name;
} unknown_operand;

typedef struct {
  Str name;
  int addr;
} label;

typedef struct {
  label labels[LABEL_TABLE_CAPACITY];
  size_t size;
  unknown_operand unknown_operands[UNKNOWN_OP_TABLE_CAP];
  size_t uo_size;
} label_table;

typedef struct {
  unsigned char memory[vm_capacity];
  size_t capacity;
  size_t size;

  instruction program[1000];
  size_t program_capacity;
  size_t program_size;
  label_table lb;
} choi_asm;

int has_label(label_table *lb_t, Str name);
void push_label(label_table *lb_t, label l);
int get_label_addr(label_table *lb_t, Str name);
int parse_register(Str str);
instruction parse_line(Str ln, label_table *lb, size_t ip);
void parse_special_operands(Str ln, choi_asm *ca, size_t ip, Arena *strArena);
size_t parse_file(Str source, choi_asm *ca, choi_header *ch, Arena *strArena);
#endif