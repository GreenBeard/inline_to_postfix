#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "./lexical_analyzer.h"
#include "./token_analyzer.h"

void token_list_free(token_list_t* token_list) {
  for (long long i = 0; i < token_list->list.size; ++i) {
    struct infix_token* token = token_list->list.items[i];
    free(token->token);
    free(token);
  }
  free(token_list->list.items);
  free(token_list);
}

void print_tokens(token_list_t* token_list) {
  for (long long i = 0; i < token_list->list.size; ++i) {
    struct infix_token* token = token_list->list.items[i];
    printf("Token: [%s] Type: [%d]\n", token->token, token->type);
  }
}

char get_op_char(enum infix_operator_node_type type) {
  switch (type) {
    case infix_operator_node_add:
      return '+';
    case infix_operator_node_subtract:
      return '-';
    case infix_operator_node_multiply:
      return '*';
    case infix_operator_node_divide:
      return '/';
    case infix_operator_node_power:
      return '^';
    default:
      assert(false);
  }
}

void infix_tree_free(struct infix_tree_node* tree) {
  if (tree == NULL) {
    /* do nothing */
  } else {
    switch(tree->type) {
      case infix_tree_node_parentheses:
        infix_tree_free(tree->node.paren.child);
        break;
      case infix_tree_node_operator:
        infix_tree_free(tree->node.op.left);
        infix_tree_free(tree->node.op.right);
        break;
      case infix_tree_node_number:
      case infix_tree_node_blank:
        break;
      default:
        assert(false);
    }
    free(tree);
  }
}

void print_postfix(struct infix_tree_node* tree) {
  if (tree == NULL) {
    printf("NULL");
  } else if (tree->type == infix_tree_node_blank) {
    printf("BLANK");
  } else if (tree->type == infix_tree_node_parentheses) {
    print_postfix(tree->node.paren.child);
  } else if (tree->type == infix_tree_node_number) {
    printf("%lld", tree->node.num.number);
  } else if (tree->type == infix_tree_node_operator) {
    print_postfix(tree->node.op.left);
    printf(" ");
    print_postfix(tree->node.op.right);
    printf(" %c", get_op_char(tree->node.op.type));
  }
}

int main(int argc, char** argv) {
  token_list_t* token_list = malloc(sizeof(token_list_t));
  printf("Performing lexical analysis\n");
  if (token_list != NULL && lexical_analyze(stdin, token_list)) {
    printf("\n");
    //print_tokens(token_list);
    printf("Performing token analysis\n");
    struct infix_tree_node* tree;
    if (token_analyze(token_list, &tree)) {
      print_postfix(tree);
      printf("\n");
      infix_tree_free(tree);
    }
    token_list_free(token_list);
  }
  return 0;
}
