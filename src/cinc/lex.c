#include "lex.h"
#include "strio.h"
#include "strv.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define save(cls) cls->tkbuffer[pos++] = cls->source.data[cls->pos]
#define next(cls) cls->current = cls->source.data[++cls->pos]

token reserved[] = {
    {TK_VAR, "var"},
};

void syntax_error(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

int cinc_isdigit(char ch) {
  return '0' <= ch && ch <= '9';
}

int cinc_isalpha(char ch) {
  return 'A' <= ch && ch <= 'z';
}

strv read_numeral(cinc_lexer_state *cls) {
  int pos = 0;

  while (cinc_isdigit(cls->current)) {
    assert(pos <= 256);
    save(cls);
    next(cls);
  }
  cls->tkbuffer[pos] = '\0';

  return tostrv(cls->tkbuffer);
}

strv read_string(cinc_lexer_state *cls) {
  int pos = 0;

  next(cls);

  while (cls->current != '"') {
    assert(pos <= 256);
    save(cls);
    next(cls);
  }

  next(cls);

  cls->tkbuffer[pos] = '\0';

  return tostrv(cls->tkbuffer);
}

int is_reserved(strv buffer) {
  for (int i = 0; i < Reserved_count; i++) {
    if (strvcmp(tostrv(reserved[i].value), buffer)) {
      return reserved[i].type;
    }
  }

  return -1;
}

strv read_identifier(cinc_lexer_state *cls) {
  int pos = 0;

  while (cinc_isalpha(cls->current)) {
    assert(pos <= 256);
    save(cls);
    next(cls);
  }
  cls->tkbuffer[pos] = '\0';

  return tostrv(cls->tkbuffer);
}

token cinc_lex(cinc_lexer_state *cls) {
  if (cls->pos == cls->source.size) {
    return (token){.type = EFO, .value = "<eof>"};
  }

  switch (cls->current) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    cstr num = read_numeral(cls).data;
    return (token){.type = TK_NUM, .value = num};
    break;
  }
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z': {
    strv idenifier = read_identifier(cls);
    // when no recognition it will always be a name
    // just in case it fails.
    int kind = is_reserved(idenifier);
    if (kind < 0) {
      return (token){.type = TK_NAME, .value = idenifier.data};
    } else {
      return (token){.type = kind, .value = idenifier.data};
    }
    break;
  }
  case '+':
    next(cls);
    return (token){.type = TK_OPERATOR, .value = "+"};
  case '-':
    next(cls);
    return (token){.type = TK_OPERATOR, .value = "-"};
  case '*':
    next(cls);
    return (token){.type = TK_OPERATOR, .value = "*"};
  case '/':
    next(cls);
    return (token){.type = TK_OPERATOR, .value = "/"};
  case '\n':
    cls->line++;
    next(cls);
    return cinc_lex(cls);
  case ' ':
  case '\r':
    next(cls);
    // parse next token instead
    return cinc_lex(cls);
  case ';':
    next(cls);
    return (token){.type = TK_SEMICOLON, .value = ";"};
  case '=':
    next(cls);
    return (token){.type = TK_EQUALS, .value = "="};
  case '(':
    next(cls);
    return (token){.type = TK_LPAREN, .value = "("};
  case ')':
    next(cls);
    return (token){.type = TK_RPAREN, .value = ")"};
  case ',':
    next(cls);
    return (token){.type = TK_COMMA, .value = ","};
  case '\"':
    return (token){.type = TK_STRING, .value = read_string(cls).data};
  default:
    if (cls->pos >= cls->source.size) {
      return (token){.type = EFO, .value = "<EOF>"};
    } else {
      syntax_error("[cinc] Error: unrecognized character at line %d. '%c'\n",
                   cls->line, cls->current);
      exit(1);
    }
    break;
  }

  return (token){.type = EFO, .value = "<EOF>"};
}