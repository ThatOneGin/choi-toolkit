#ifndef CHOI_SYSCALLS
#define CHOI_SYSCALLS
#include "choivm.h"

typedef enum {
  choi_write = 1, // R1: 1 | R2: fd | R3: msglen
  choi_read, // R1: 2 | R2: fd | R3: n
  choi_open, // R1: 3 | R2: mode | R3: filename_size | filename in memory
  choi_close // R1: 4 | R2: fd
} SYSCALL_NUMBERS;

int check_for_syscall_kind(gsb_vm *vm);
#endif
