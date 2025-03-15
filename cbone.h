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
#include <direct.h>
#include <windows.h>
typedef HANDLE fd;
#define path_sep "\\"
#endif

#ifdef __GNUC__
  #ifndef cc
    #define cc "gcc"
  #endif
#endif

#ifdef __clang__
  #ifndef cc
    #define cc "clang"
  #endif
#endif

#ifdef __MSC_VER
  #ifndef cc
    #define cc "cl.exe"
  #endif
#endif

#ifndef cc
  #ifndef cc
    #define cc "cc"
  #endif
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char **items;
  size_t size;
  size_t capacity;
} Str_Array;

typedef struct {
  Str_Array data;
} Cmd;

int cbone_run_cmd(Cmd arg);
int cbone_modified_after(char *f1, char *f2);
Str_Array cbone_make_str_array(char *first, ...);
char *cbone_concat_str_array(char *delim, Str_Array s);
void cbone_print_cmd(char *cmd);
void cbone_info_cmd(char *msg);
void cbone_assert_with_errmsg(int expr, char *errmsg);
int cbone_dir_exists(Str_Array Array_path);
int cbone_mkdir(char *path);
int cbone_rmdir(char *path);

/*
**Dynamic arrays for utilities

DA_DEFAULT_CAP: minimum capacity for arrays (customizable)
DA_ASSERT: assertion method used in errors

DA_FREE: free an dynamic array.

DA_PUSH: push an element to the front of an array

DA_POP: remove an element on the front of the array.

DA_PUSH_AT: push an element at position (adjust others to fit)

DA_POP_AT: remove an element at position (adjust others to fill)

DA_GET: gets an element at given position, if the position is greater
than the size, it will give the last element. Otherwise if it underflows, the
first.
*/

#ifndef DA_DEFAULT_CAP
#define DA_DEFAULT_CAP 64
#endif

#ifndef DA_ASSERT
#define DA_ASSERT assert
#endif

#define LOG(pref, str) (fprintf(stdout, pref str))
#define ERROR(msg) LOG("Error:", msg)

#define DA_FREE(arr)                                                           \
  do {                                                                         \
    if (arr.size > 0 && arr.capacity > 0) {                                    \
      free(arr.items);                                                         \
    }                                                                          \
  } while (0)

#define DA_PUSH(arr, elm)                                                    \
do {                                                                         \
  if (arr.size >= arr.capacity) {                                            \
    if (arr.capacity == 0)                                                   \
      arr.capacity = DA_DEFAULT_CAP;                                         \
    else                                                                     \
      arr.capacity *= 2;                                                     \
    arr.items = realloc(arr.items, arr.capacity * sizeof(*arr.items));       \
    if (arr.items == NULL) {                                                 \
      ERROR("DA_PUSH fail: Realloc Error.");                                 \
      exit(1);                                                               \
    }                                                                        \
  }                                                                          \
  arr.items[arr.size++] = (elm);                                             \
} while (0)

#define DA_POP(arr)                                                          \
do {                                                                         \
  if (arr.capacity > 0 && arr.size > 0) {                                    \
    arr.size--;                                                              \
  }                                                                          \
} while (0)

#define DA_POP_AT(arr, pos)                                                  \
do {                                                                         \
  if ((pos) < arr.size) {                                                    \
    for (size_t i = (pos); i < (size_t)arr.size - 1; i++) {                  \
      arr.items[i] = arr.items[i + 1];                                       \
    }                                                                        \
    arr.size--;                                                              \
  }                                                                          \
} while (0)

#define DA_GET(arr, pos) ((pos) >= 0 ? arr.size > (pos) ? arr.items[(pos)] : arr.items[arr.size - 1] : arr.items[0])

#define DA_PUSH_AT(arr, elm, pos)                                            \
do {                                                                         \
  if (arr.size + (pos) < arr.capacity) {                                     \
    for (size_t i = arr.size; i > pos; i--) {                                \
      arr.items[i] = arr.items[i - 1];                                       \
    }                                                                        \
    arr.items[pos] = elm;                                                    \
  }                                                                          \
} while (0)

/* Implementation section */
#ifdef CBONE_IMPL

#define PATH(...) cbone_concat_str_array(path_sep, cbone_make_str_array(__VA_ARGS__, NULL))
#define CONCAT(...) cbone_concat_str_array("", cbone_make_str_array(__VA_ARGS__, NULL))
#define CMD(...)                                                               \
  do {                                                                         \
    Cmd arg = {.data = cbone_make_str_array(__VA_ARGS__, NULL)};               \
    cbone_run_cmd(arg);                                                              \
    free(arg.data.items);                                                      \
  } while (0)

char *str_concat(char *s1, char *s2) {
  char *buffer = malloc(strlen(s1) + strlen(s2) + 1);
  cbone_assert_with_errmsg(buffer != NULL, "Couldn't concat string.");

  strncat(buffer, s1, strlen(s1));
  strncat(buffer, s2, strlen(s2));

  return buffer;
}

void cbone_assert_with_errmsg(int expr, char *errmsg) {
  if (!expr) {
    printf("[ERROR]: %s\n", errmsg);
    exit(1);
  }
}

void cbone_info_cmd(char *msg) { printf("[INFO]: %s\n", msg); }

void cbone_print_cmd(char *cmd) { printf("[CMD]: %s\n", cmd); }

Str_Array cbone_make_str_array(char *first, ...) {
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

  result.items = malloc(sizeof(result.items[0]) * result.size);
  cbone_assert_with_errmsg(result.items != NULL,
                     "Couldn't allocate memory for string array.");

  result.size = 0;

  result.items[result.size++] = first;

  va_start(args, first);
  for (char *next = va_arg(args, char *); next != NULL;
       next = va_arg(args, char *)) {
    result.items[result.size++] = next;
  }
  va_end(args);

  return result;
}

Str_Array str_array_push(Str_Array s_arr, char *str) {
  Str_Array result = {.size = s_arr.size + 1};

  result.items = malloc(result.size);
  cbone_assert_with_errmsg(result.items != NULL,
                     "Coudln't push string to string array.");

  memcpy(result.items, s_arr.items, s_arr.size);

  result.items[s_arr.size] = str;

  return result;
}

char *cbone_concat_str_array(char *sep, Str_Array s) {
  if (s.size == 0) {
    return "";
  }

  size_t sep_len = strlen(sep);
  size_t len = 0;
  for (size_t i = 0; i < s.size; i++) {
    len += strlen(s.items[i]);
    if (i < s.size - 1) {
      len += sep_len;
    }
  }

  char *result = malloc((len + 1) * sizeof(char));
  cbone_assert_with_errmsg(result != NULL, "Couldn't join string array.");

  result[0] = '\0';

  for (size_t i = 0; i < s.size; i++) {
    strcat(result, s.items[i]);
    if (i < s.size - 1) {
      strcat(result, sep);
    }
  }

  return result;
}

int cbone_run_cmd(Cmd arg) {
  char *cmd = cbone_concat_str_array(" ", arg.data);
  cbone_print_cmd(cmd);
#ifndef _WIN32
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    return 1;
  } else if (pid == 0) {
    DA_PUSH(arg.data, NULL);
    execvp(arg.data.items[0], (char *const *)arg.data.items);

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

int cbone_modified_after(char *f1, char *f2) {
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

#define REBUILD_SELF(argc, argv)                                             \
do {                                                                         \
  cbone_assert_with_errmsg(argc >= 1, "no args... how?");                    \
  char *src_file = __FILE__;                                                 \
  char *target = argv[0];                                                    \
  if (cbone_modified_after(src_file, target)) {                              \
    CMD(cc, "-o", target, src_file);                                         \
  }                                                                          \
} while (0)

/*
Returns if given path as an array of strings is a directory.
Return values:
  0: don't exists.
  1: exists.
  2: not a directory, but exists.*/
int cbone_dir_exists(Str_Array Array_path) {
  char *path = cbone_concat_str_array(path_sep, Array_path);
  int result;
#ifdef _WIN32
  DWORD attr = GetFileAttributes(path);
  if (attr == INVALID_FILE_ATTRIBUTES) {
    // don't exists
    result = 0;
  } else if (attr & FILE_ATTRIBUTE_DIRECTORY) {
    // is a directory
    result = 1;
  } else {
    // is a file
    result = 2;
  }
#else
  struct stat statbuf;
  if (stat(path, &statbuf) == 0) {
    if (S_ISDIR(statbuf.st_mode)) {
      // is a directory
      result = 1;
    } else {
      // is a file
      result = 2;
    }
  } else {
    // don't exists
    result = 0;
  }
#endif
  free(path);
  return result;
}

// makes a folder with given path in form of a string.
int cbone_mkdir(char *path) {
  int result;

#ifdef _WIN32
  result = _mkdir(path);
#else
  result = mkdir(path, 0755);
#endif
  return result;
}

// this function only delete empty directories
// so if using to delete a folder with files, you should
// delete all files in that folder
int cbone_rmdir(char *path) {
  #ifdef _WIN32
    return RemoveDirectory(path);
  #else
    return rmdir(path) == 0;
  #endif
}

#endif // CBONE_IMPL
#endif // CBONE_H