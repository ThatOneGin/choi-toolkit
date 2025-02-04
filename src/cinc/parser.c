#include "parser.h"
#include "cinc.h"
#include "lex.h"
#include "mem.h"
#include "strio.h"
#include "strv.h"
#include <stdio.h>
#include <string.h>

cstr type_as_str(int type) {
  switch (type) {
  case TK_VAR:
    return "keyword var";
  // non - keywords
  case TK_OPERATOR:
    return "binary operator";
  case TK_SEMICOLON:
  case TK_EQUALS:
    return "<ponctuation>";
  case TK_NUM:
    return "<number>";
  case TK_NAME:
    return "<identifier>";
  case TK_LPAREN:
    return "<TK_LPAREN>";
  case TK_RPAREN:
    return "<TK_RPAREN>";
  case EFO:
    return "<EOF>";
  }

  return "Non type";
}

token at(cinc_parser_state *p) { return p->current; }

token eat(cinc_parser_state *p) {
  token prev = p->current;
  p->current = cinc_lex(p->lex);
  return prev;
}

token expect(cinc_parser_state *p, int type) {
  token prev = eat(p);
  if (prev.type != type) {
    printf("[cinc] Error: Expected type `%s` but found token of type: `%s`.\n",
           type_as_str(type), type_as_str(prev.type));
    cinc_close(1);
  }
  return prev;
}

cinc_expr parse_primary_expr(cinc_parser_state *p) {
  token tk = at(p);

  switch (tk.type) {
  case TK_NAME:
    return (cinc_expr){.kind = NAME, .value = {.as_name = eat(p).value}};
    break;
  case TK_NUM:
    return (cinc_expr){.kind = NUM_LITERAL,
                       .value = {.as_num_literal = strv_atoi(eat(p).value)}};
  case TK_STRING: {
    cstr strbuff = cinc_alloc_from_arena(sizeof(char) * tostrv(tk.value).size + 1);
    strcpy(strbuff, tostrv(tk.value).data);
    eat(p);

    return (cinc_expr){.kind = STRING_LITERAL,
                       .value = {.as_string = tostrv(strbuff)}};
  }
  case TK_LPAREN:
    eat(p);
    cinc_expr expr = parse_expr(p);
    expect(p, TK_RPAREN);
    return expr;
  default:
    printf("At token '%s'\n", tk.value);
    cinc_error("shouldn't be handled as primary expression.");
    cinc_close(1);
    break;
  }
}

cinc_expr parse_multiplicitave_expr(cinc_parser_state *p) {
  cinc_expr left = parse_primary_expr(p);

  while (strcmp(at(p).value, "*") == 0 || strcmp(at(p).value, "/") == 0) {
    char operator= eat(p).value[0];
    cinc_expr right = parse_primary_expr(p);

    cinc_expr tmp = left;
    left = (cinc_expr){
        .kind = BINOP,
        .value = {.as_binary_op = cinc_alloc_from_arena(sizeof(cinc_binop))}};
    left.value.as_binary_op->lhs = tmp;
    left.value.as_binary_op->rhs = right;
    left.value.as_binary_op->op = operator;
  }

  return left;
}

cinc_expr parse_additive_expr(cinc_parser_state *p) {
  cinc_expr left = parse_multiplicitave_expr(p);

  while (strcmp(at(p).value, "+") == 0 || strcmp(at(p).value, "-") == 0) {
    char operator= eat(p).value[0];
    cinc_expr right = parse_multiplicitave_expr(p);

    cinc_expr tmp = left;
    left = (cinc_expr){
        .kind = BINOP,
        .value = {.as_binary_op = cinc_alloc_from_arena(sizeof(cinc_binop))}};
    left.value.as_binary_op->lhs = tmp;
    left.value.as_binary_op->rhs = right;
    left.value.as_binary_op->op = operator;
  }

  return left;
}

cinc_expr parse_expr(cinc_parser_state *p) {
  switch (at(p).type) {
  case TK_NAME:
    return parse_function_call(p);
  default:
    return parse_additive_expr(p);
  }
}

cinc_expr parse_stmt(cinc_parser_state *p) {
  cinc_expr expr;

  switch (at(p).type) {
  case TK_VAR:
    expr = parse_variable_declaration(p);
    break;
  default:
    expr = parse_expr(p);
    break;
  }

  expect(p, TK_SEMICOLON);

  return expr;
}

cinc_expr parse_variable_declaration(cinc_parser_state *p) {
  expect(p, TK_VAR);
  char *name = expect(p, TK_NAME).value;
  expect(p, TK_EQUALS);
  cinc_expr value = parse_expr(p);

  cinc_expr var =
      (cinc_expr){.kind = VAR_DECLARATION,
                  .value.as_var = cinc_alloc_from_arena(sizeof(cinc_var))};

  var.value.as_var->value = value;
  var.value.as_var->name = tostrv(name);

  return var;
}

cinc_expr parse_function_call(cinc_parser_state *p) {

  token caller = expect(p, TK_NAME);

  if (at(p).type == TK_LPAREN) {
    cinc_expr fn_call =
        (cinc_expr){.kind = FN_CALL,
                    .value.as_fn_call = cinc_alloc_from_arena(sizeof(cinc_fn_call))};
    asfncall(fn_call)->caller.data =
        cinc_alloc_from_arena(sizeof(char) * tostrv(caller.value).size + 1);
    strcpy(asfncall(fn_call)->caller.data, tostrv(caller.value).data);
    asfncall(fn_call)->caller.size = tostrv(caller.value).size;

    expect(p, TK_LPAREN);

    size_t pos = 0;
    cinc_expr args[120];

    while (at(p).type != TK_RPAREN) {
      args[pos++] = parse_expr(p);
      if (at(p).type == TK_COMMA) {
        eat(p);
      }
    }

    asfncall(fn_call)->args = cinc_alloc_from_arena(sizeof(cinc_expr) * pos);
    asfncall(fn_call)->arglen = pos;

    memcpy(asfncall(fn_call)->args, args, sizeof(cinc_expr) * pos);

    expect(p, TK_RPAREN);

    return fn_call;
  } else {
    return (cinc_expr){.kind = NAME, .value = {.as_name = caller.value}};
  }
}