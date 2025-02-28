# Choi VM

Choivm is a virtual machine with a goal of being small and portable.
It has its own assembly language called casm (choi assembly, a interface with pratically no abstraction for its bytecode)

## Building:

Compile cbone.c and execute.

```shell
$ cc -o cbone cbone.c && ./cbone
```

Executing cbone will generate these following 3 executables:

choivm: the virtual machine bytecode interpreter.

choiasm: the assembly compiler.

choi_disasm: the disassembler.

## Syntax:

choi_assembly:

```
@proc main
  @mem R1 "Hello, choi!"
  write R1
```

## quickstart

### Operands

```push```

push a value to a register.

```
  push R1 69
```

```mov```

move values between registers.

```
  mov R1 R2
```

```add```

sum two registers as integers.

```
  add R1 R2
```

```sub```

subtract two registers as integers

```
  sub R1 R2
```

```div```

divide two registers as integers.

```
  div R1 R2
```

```mul```

multiply two registers as integers.

```
  mul R1 R2
```

```dump```

dump a register to stdout as integer

```
  dump R1
```

```jmp```

Change the instruction pointer to a certain location.

```
  jmp hello-procedure
```

```jz```

Change the instruction pointer to a certain location if the last element in the stack is zero.

```
  jz hello-procedure
```

```jnz```

Change the instruction pointer to a certain location if the last element in the stack is not zero.

```
  jnz hello-procedure
```

```call```

Call a procedure, operand call makes the use of ret instruction possible by pushing the current address in the program.

```
  call hello-procedure
```

```drop```

set a register to zero

```
  drop R1
```

```write```

writes N bytes of an certain address in the constant pool

```
  write R1
```

```syscall```

looks up at the registers and execute a special function based on the R1 register.

```
  push R1 1
  push R2 0
  @mem R3 "Hello, world!"
  syscall
```

```halt```

stop execution and ends the program.

```
  halt
```

```cmp```

subtract two registers and push result to the stack

```
  cmp R1 R2
```

```inc```

increment a register by one

```
  inc R3
```

### Special operands (operate stack, pool and procedures)

```@proc```

declares a procedure, while not visible at bytecode itself, it can be useful for cross referencing blocks

```
  @proc hello-procedure
```

```@mem```

push pointer/string to constant pool and its length to a register.


```
  @mem R1 "Hello, world"
```

```@pop```

pops an element of the stack to a register

```
  @pop R1
```

```@push```

push a value to the stack

```
  @push 69
```

## confusion

operators preceded with '@' are different.

push is not the same as @push

push will push a value to a register

@push will push a value to the stack

## Todos

- [ ] meta programming features to the assembly.

- [ ] handle strings like a sane person.

- [ ] a debugger for the bytecode.