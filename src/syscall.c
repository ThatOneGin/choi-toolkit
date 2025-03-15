#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"
#include "choivm.h"

// R1: 1 | R2: fd | R3: msglen
// R1: 2 | R2: fd | R3: n
// R1: 3 | R2: mode | R3: filename_size | filename in memory
// R1: 4 | R2: fd

int check_for_syscall_kind(gsb_vm *vm) {
  char filename[256];

  // TODO: syscalls for windows
  switch (vm->registers[0].as_int) {
    case choi_write:
      vm->registers[0].as_int = write(vm->registers[1].as_int, vm->memory + pop_vm(vm), vm->registers[2].as_int);
      return 0;
    case choi_read:
      push_vm(vm, vm->size);
      vm->registers[0].as_int = read(vm->registers[1].as_int, vm->memory + vm->size, vm->registers[2].as_int);
      vm->size += (size_t) vm->registers[2].as_int;
      return 0;
    case choi_open:
      sprintf(filename, "%.*s", vm->registers[2].as_int, vm->memory + pop_vm(vm));
      vm->registers[0].as_int = open(filename, vm->registers[1].as_int);
      break;
    case choi_close:
      close(vm->registers[1].as_int);
      return 0;
      break;
  }

  return -1;
}
