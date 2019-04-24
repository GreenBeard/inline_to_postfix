#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <stdbool.h>
#include <stddef.h>

#include "./simple_list.h"

enum infix_token_type {
  infix_token_none,
  infix_token_number,
  infix_token_operator,
  infix_token_parentheses
};

typedef struct token_list {
  struct doubling_list list;
} token_list_t;

struct infix_token {
  enum infix_token_type type;
  size_t token_len_cap;
  char* token;
};

bool lexical_analyze(FILE* file, token_list_t* token_list);

#endif
