#ifndef LEX_H
#define LEX_H
#include "strio.h"

enum tk_kind {
	TK_VAR,
	Reserved_count,
  TK_OPERATOR,
  TK_SEMICOLON,
  TK_EQUALS,
	TK_NUM,
  TK_NAME,
  TK_STRING,
  TK_COMMA,
  TK_RPAREN,
  TK_LPAREN,
  EFO
};

typedef struct {
	int type;
	char *value;
} token;

typedef struct {
  const strv source;
  char current;
  int pos;
  int line;
  char tkbuffer[256];
} cinc_lexer_state;

int cinc_isdigit(char ch);
int cinc_isalpha(char ch);
int is_reserved(strv buffer);
token cinc_lex(cinc_lexer_state *cls);
#endif