#!/bin/bash

CFLAGS=" -ggdb -Wall -Wextra -Werror -pedantic-errors -std=c99"

cd build
clang $CFLAGS ../src/choiasm.c -o choiasm
clang $CFLAGS ../src/choivm.c -o choivm
clang $CFLAGS ../src/choi_disasm.c -o choi_disasm
cd ..
