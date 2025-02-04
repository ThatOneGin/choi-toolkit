#ifndef choi_str_h
#define choi_str_h
#include <stdio.h>
#include "choivm.h"
#include "arena.h"

#define LABEL_TABLE_CAPACITY 100
#define UNKNOWN_OP_TABLE_CAP 100

typedef struct {
  char *data;
  size_t size;
} Str;

typedef struct {
  Str name;
  int addr;
} label;

typedef struct {
  int addr;
  Str name;
} unknown_operand;

typedef struct {
  label labels[LABEL_TABLE_CAPACITY];
  size_t size;
  unknown_operand unknown_operands[UNKNOWN_OP_TABLE_CAP];
  size_t uo_size;
} label_table;

typedef struct {
  unsigned char memory[vm_memory_capacity];
  size_t memory_capacity;
  size_t memory_size;

  instruction program[1000];
  size_t program_capacity;
  size_t program_size;
  label_table lb;
} choi_asm;

int has_label(label_table *lb_t, Str name);
void push_label(label_table *lb_t, label l);
int get_label_addr(label_table *lb_t, Str name);
int parse_register(Str str);
Str str_capture(Str s, char delim);
Str to_str(char *data) ;
int str_cmp(Str a, Str b);
Str str_split(Str *s, char delim);
int str_atoi(Str s);
Str str_triml(Str s);
Str str_trimr(Str s);
Str str_trimlr(Str s);
Str str_trims(Str str);
Str str_cpy(Str s);
instruction parse_line(Str ln, label_table *lb, size_t ip);
void parse_special_operands(Str ln, choi_asm *ca, size_t ip, Arena *strArena);
size_t parse_file(Str source, choi_asm *ca, choi_header *ch, Arena *strArena);
#endif
