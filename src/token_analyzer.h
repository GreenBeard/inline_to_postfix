#ifndef TOKEN_ANALYZER_H
#define TOKEN_ANALYZER_H

#include "./lexical_analyzer.h"

enum infix_tree_node_type {
  infix_tree_node_number,
  infix_tree_node_operator,
  infix_tree_node_parentheses,
  infix_tree_node_blank
};

struct infix_tree_node;

enum infix_operator_node_type {
  infix_operator_node_add,
  infix_operator_node_subtract,
  infix_operator_node_multiply,
  infix_operator_node_divide,
  infix_operator_node_power
};

struct infix_operator_parentheses {
  struct infix_tree_node* child;
};

struct infix_operator_node {
  enum infix_operator_node_type type;
  struct infix_tree_node* left;
  struct infix_tree_node* right;
};

struct infix_number_node {
  long long number;
};

union infix_tree_vague_node {
  struct infix_number_node num;
  struct infix_operator_node op;
  struct infix_operator_parentheses paren;
};

struct infix_tree_node {
  enum infix_tree_node_type type;
  union infix_tree_vague_node node;
};

struct infix_tree_node_list {
  struct doubling_list list;
};

bool token_analyze(token_list_t* token_list, struct infix_tree_node** return_top_node);

#endif
