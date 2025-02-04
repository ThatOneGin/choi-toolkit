/*
MIT License

Copyright (c) 2025 ThatOneGin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CBONE_H
#define CBONE_H
#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
typedef int fd;
#define path_sep "/"
#else
#include <process.h>
#include <windows.h>
typedef HANDLE fd;
#define path_sep "\\"
#endif

#ifdef __GNUC__
  #define cc "gcc"
#endif

#ifdef __clang__
  #define cc "clang"
#endif

#ifdef __MSC_VER
  #define cc "cl.exe"
#endif

#ifndef cc
  #define cc "cc"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char **elm;
  size_t size;
} Str_Array;

typedef struct {
  Str_Array data;
} Build_Arg;

int run_cmd(Build_Arg arg);
int modified_after(char *f1, char *f2);
Str_Array make_str_array(char *first, ...);
char *concat_str_array(char *delim, Str_Array s);
void print_cmd(char *cmd);
void info_cmd(char *msg);
void assert_with_errmsg(int expr, char *errmsg);

#ifdef CBONE_IMPL

#define PATH(...) concat_str_array(path_sep, make_str_array(__VA_ARGS__, NULL))
#define CONCAT(...) concat_str_array("", make_str_array(__VA_ARGS__, NULL))
#define CMD(...)                                                               \
  do {                                                                         \
    Build_Arg arg = {.data = make_str_array(__VA_ARGS__, NULL)};               \
    run_cmd(arg);                                                              \
    free(arg.data.elm);                                                        \
  } while (0)

char *str_concat(char *s1, char *s2) {
  char *buffer = malloc(strlen(s1) + strlen(s2) + 1);
  assert_with_errmsg(buffer != NULL, "Couldn't concat string.");

  strncat(buffer, s1, strlen(s1));
  strncat(buffer, s2, strlen(s2));

  return buffer;
}

void assert_with_errmsg(int expr, char *errmsg) {
  if (!expr) {
    printf("[ERROR]: %s\n", errmsg);
    exit(1);
  }
}

void info_cmd(char *msg) { printf("[INFO]: %s\n", msg); }

void print_cmd(char *cmd) { printf("[CMD]: %s\n", cmd); }

Str_Array make_str_array(char *first, ...) {
  Str_Array result = {0};

  if (first == NULL) {
    return result;
  }

  result.size += 1;

  va_list args;
  va_start(args, first);
  for (char *next = va_arg(args, char *); next != NULL;
       next = va_arg(args, char *)) {
    result.size += 1;
  }
  va_end(args);

  result.elm = malloc(sizeof(result.elm[0]) * result.size + 1);
  assert_with_errmsg(result.elm != NULL,
                     "Couldn't allocate memory for string array.");

  result.size = 0;

  result.elm[result.size++] = first;

  va_start(args, first);
  for (char *next = va_arg(args, char *); next != NULL;
      next = va_arg(args, char *)) {
    result.elm[result.size++] = next;
  }
  va_end(args);

  return result;
}

Str_Array str_array_push(Str_Array s_arr, char *str) {
  Str_Array result = {.size = s_arr.size};

  result.elm = malloc(result.size);
  assert_with_errmsg(result.elm != NULL,
                     "Coudln't push string to string array.");

  memcpy(result.elm, s_arr.elm, s_arr.size);

  result.elm[s_arr.size] = str;

  return result;
}

char *concat_str_array(char *sep, Str_Array s) {
  if (s.size == 0) {
    return "";
  }

  size_t sep_len = strlen(sep);
  size_t len = 0;
  for (size_t i = 0; i < s.size; i++) {
    len += strlen(s.elm[i]);
    if (i < s.size - 1) {
      len += sep_len;
    }
  }

  char *result = malloc((len + 1) * sizeof(char));
  assert_with_errmsg(result != NULL, "Couldn't join string array.");

  result[0] = '\0';

  for (size_t i = 0; i < s.size; i++) {
    strcat(result, s.elm[i]);
    if (i < s.size - 1) {
      strcat(result, sep);
    }
  }

  return result;
}

int run_cmd(Build_Arg arg) {
  char *cmd = concat_str_array(" ", arg.data);
  print_cmd(cmd);
#ifndef _WIN32
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    return 1;
  } else if (pid == 0) {
    execvp(arg.data.elm[0], (char *const *)arg.data.elm);

    perror("execvp");
    return 1;
  } else {
    wait(NULL);
  }
#else
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));

  si.cb = sizeof(si);

  if (CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  } else {
    printf("Error creating process: %ld\n", GetLastError());
  }
#endif
  free(cmd);
  return 0;
}

fd open_file(char *path) {
#ifndef _WIN32
  fd fl = open(path, O_RDONLY);

  if (fl < 0) {
    perror("Couldn't open file");
    exit(1);
  }
#else
  fd fl = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL, NULL);

  if (fl == INVALID_HANDLE_VALUE) {
    printf("Couldn't open %s: %s", path, GetLastErrorAsString());
    exit(1);
  }
#endif
  return fl;
}

void fd_close(fd f) {
#ifdef _WIN32
  CloseHandle(f);
#else
  close(f);
#endif
}

int modified_after(char *f1, char *f2) {
#ifdef _WIN32
  FILETIME file1_time, file2_time;
  fd file1 = open_file(f1);
  fd file2 = open_file(f2);

  if (!GetFileTime(file1, NULL, NULL, &file1_time)) {
    printf("Could not get time of %s: %s", path1, GetLastErrorAsString());
    exit(1);
  }
  fd_close(file1);

  if (!GetFileTime(file2, NULL, NULL, &file2_time)) {
    printf("Could not get time of %s: %s", path2, GetLastErrorAsString());
    exit(1);
  }
  fd_close(file2);

  return CompareFileTime(&file1_time, &file2_time) == 1;
#else
  struct stat flstat_buffer = {0};

  if (stat(f1, &flstat_buffer) < 0) {
    perror("Could get file time");
  }

  int f1_time = flstat_buffer.st_mtime;

  if (stat(f2, &flstat_buffer) < 0) {
    perror("Could get file time");
  }

  int f2_time = flstat_buffer.st_mtime;

  return f1_time > f2_time;
#endif
}

#define REBUILD_SELF(argc, argv)                                               \
  {                                                                            \
    assert_with_errmsg(argc >= 1, "no args... how?");                          \
    char *src_file = __FILE__;                                                 \
    char *target = argv[0];                                                    \
                                                                               \
    if (modified_after(src_file, target)) {                                    \
      CMD(cc, "-o", target, src_file);                                         \
    }                                                                          \
  }
#endif // CBONE_IMPL
#endif // CBONE_H