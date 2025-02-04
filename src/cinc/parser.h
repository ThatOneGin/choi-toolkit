#ifndef PARSER_H
#define PARSER_H
#include "lex.h"
#include "mem.h"
#include "state.h"
#include "strv.h"
#include <stdint.h>

#define maxtreelen 1200
#define asint(node) ((node).value.as_num_literal)
#define asname(node) ((node).value.as_name)
#define asbinop(node) ((node).value.as_binary_op)
#define asvar(node) ((node).value.as_var)
#define asfncall(node) ((node).value.as_fn_call)

typedef enum {
  NUM_LITERAL,
  STRING_LITERAL,
  NAME,
  VAR_DECLARATION,
  BINOP,
  FN_CALL
} cinc_expr_kind;

typedef union cinc_expr_value cinc_expr_value;
typedef struct cinc_expr cinc_expr;
typedef struct cinc_binop cinc_binop;
typedef struct cinc_var cinc_var;
typedef struct cinc_fn_call cinc_fn_call;

union cinc_expr_value {
  // hate these cases ngl
  cinc_binop *as_binary_op;
  cinc_var *as_var;
  cinc_fn_call *as_fn_call;
  uint8_t as_num_literal;
  strv as_name;
  strv as_string;
};

struct cinc_expr {
  cinc_expr_kind kind;
  cinc_expr_value value;
};

struct cinc_binop {
  char op;
  cinc_expr lhs;
  cinc_expr rhs;
};

struct cinc_var {
  strv name;
  cinc_expr value;
};

struct cinc_fn_call {
  strv caller;
  cinc_expr *args;
  size_t arglen;
};

typedef struct {
  cinc_lexer_state *lex;
  token current;
  struct cinc_state cs;
} cinc_parser_state;

cstr type_as_str(int type);
token at(cinc_parser_state *p);
token eat(cinc_parser_state *p);
token expect(cinc_parser_state *p, int type);
cinc_expr parse_primary_expr(cinc_parser_state *p);
cinc_expr parse_multiplicitave_expr(cinc_parser_state *p);
cinc_expr parse_additive_expr(cinc_parser_state *p);
cinc_expr parse_variable_declaration(cinc_parser_state *p);
cinc_expr parse_function_call(cinc_parser_state *p);
cinc_expr parse_expr(cinc_parser_state *p);
#endif