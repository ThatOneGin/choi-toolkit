#include "strio.h"
#include "cinc.h"
#include "lex.h"
#include "mem.h"
#include "strv.h"
#include <stdio.h>
#include <string.h>

strv tostrv(cstr str) { return (strv){.data = str, .size = strlen(str)}; }

int strvcmp(strv s1, strv s2) {
  if (s1.size != s2.size) {
    return 0;
  } else {
    return memcmp(s1.data, s2.data, s1.size) == 0;
  }
}

strv file2strv(const char *file) {
  FILE *f = fopen(file, "r");

  if (!f || f == NULL) {
    perror("cinc_io");
    cinc_error("Couldn't open file");
    cinc_close(1);
  }

  fseek(f, 0L, SEEK_END);
  size_t sz = ftell(f);
  fseek(f, 0L, SEEK_SET);

  char *str = cinc_buffer(sz + 1);

  if (str == NULL) {
    cinc_error("Couldn't allocate memory for file.");
    cinc_close(1);
  }

  fread(str, sizeof(char), sz, f);

  fclose(f);

  return (strv){.size = sz, .data = str};
}

int strv_atoi(cstr num) {
  strv s = tostrv(num);
  int res = 0;
  for (size_t i = 0; i < s.size && cinc_isdigit(s.data[i]); i++) {
    res = res * 10 + s.data[i] - '0';
  }
  return res;
}