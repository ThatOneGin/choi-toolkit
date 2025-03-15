#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "string.h"

Str to_str(char *data) {
  return (Str) {
    .data = data,
    .size = strlen(data)
  };
}

int str_cmp(Str a, Str b) {
  if (a.size != b.size) {
    return 0;
  } else {
    return memcmp(a.data, b.data, b.size) == 0;
  }
}

int str_atoi(Str s) {
  int res = 0;
  for (size_t i = 0; i < s.size && isdigit(s.data[i]); i++) {
    res = res * 10 + s.data[i] - '0';
  }
  return res;
}

Str str_triml(Str str) {
  size_t i = 0;
  while (isspace(str.data[i]) && i < str.size) {
    i += 1;
  }

  return (Str) {
    .size = str.size - i,
    .data = str.data + i
  };
}

Str str_trimr(Str s) {
  size_t i = 0;
  while (i < s.size && isspace(s.data[s.size - 1 - i])) {
    i += 1;
  }

  return (Str) {
    .size = s.size - i,
    .data = s.data
  };
}

Str str_cpy(Str s) {
  return (Str) {
    .size = strlen(s.data),
    .data = s.data
  };
}

Str str_split(Str *str, char delim) {
  size_t i = 0;
  while (i < str->size && str->data[i] != delim) {
    i += 1;
  }

  Str res = {
    .size = i,
    .data = str->data
  };

  if (i < str->size) {
    str->size -= i + 1;
    str->data  += i + 1;
  } else {
    str->size -= i;
    str->data  += i;
  }

  return res;
}

Str str_trimlr(Str s) {
  return str_trimr(str_triml(s));
}

Str str_capture(Str s, char delim) {
  size_t i = 0;

  while (i < s.size && s.data[i] != delim) {
    i += 1;
  }
  char buffer[i+1];
  
  memcpy(buffer, s.data, i);
  buffer[i] = '\0';

  return to_str(buffer);
}

Str str_trims(Str str) {
  size_t i = 0;
  while (!isspace(str.data[i]) && i < str.size) {
    i += 1;
  }

  return (Str) {
    .size = str.size - (i + 1),
    .data = str.data + i + 1
  };
}