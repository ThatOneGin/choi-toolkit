#include "io.h"
#include "choivm.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Str openfile(char *filepath) {
  FILE *f = fopen(filepath, "r");
  if (f == NULL) {
    perror("Could not read input file");
    exit(1);
  }
  
  if (fseek(f, 0, SEEK_END) < 0) {
    perror("Could not read input file");
    exit(1);
  }

  size_t sz = ftell(f);
  if (sz <= 0) {
    perror("Could not read input file");
    exit(1);
  }
  
  char *buf = malloc(sz);
  if (buf == NULL) {
    perror("Could not alloc memory for file");
    exit(1);
  }
  
  if (fseek(f, 0, SEEK_SET) < 0) {
    perror("Could not read input file");
    exit(1);
  }
  
  size_t flsz = fread(buf, 1, sz, f);
  if (ferror(f)) {
    perror("Could not read input file");
    exit(1);
  }
  fclose(f);
  return (Str) {
    .size = flsz,
    .data = buf,
  };
}

choi_header readprogram(gsb_vm *vm, char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("Couldn't open input file");
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t c = ftell(f) / sizeof(instruction);
  rewind(f);

  instruction *program = malloc(c * sizeof(instruction));
  if (program == NULL) {
    perror("Couldn't allocate main buffer");
    exit(1);
  }
  choi_header ch = {0};
  
  fread(&ch, sizeof(ch), 1, f);
  
  if (ch.magic != magic_number) {
    printf("Corrupted file or is not a casm file.\n");
    exit(1);
  }
  if (ch.program_size >= max_program_size) {
    printf("Choivm file too long.\n");
    exit(1);
  }

  vm->ip = ch.entry;

  instruction inst = {0};
  size_t i = 0;

  while (i < ch.program_size) {
    fread(&inst, sizeof(instruction), 1, f);
    program[i] = inst;
    i++;
  }

  fread(vm->memory, sizeof(vm->memory[0]), ch.size, f);

  fclose(f);
  vm->program = program;
  vm->size = ch.size;
  return ch;
}

void writefile(const char *filename, instruction *program, choi_asm ca, choi_header ch) {
  FILE *f = fopen(filename, "wb");

  if (f == NULL) {
    perror("Error: Couldn't open file ");
    exit(1);
  }

  fwrite(&ch, sizeof(ch), 1, f);
  fwrite(program, sizeof(instruction), ch.program_size, f);
  fwrite(ca.memory, sizeof(*ca.memory), ca.size, f);

  if (ferror(f)) {
    perror("Error: Couldnt't write to file ");
    exit(1);
  }
  fclose(f);
}
