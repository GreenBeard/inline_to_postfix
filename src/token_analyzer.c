#include <execinfo.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./lexical_analyzer.h"
#include "./token_analyzer.h"
#include "./simple_list.h"

void tree_node_list_free(struct infix_tree_node_list* list) {
  free(list->list.items);
  free(list);
}

bool tree_node_list_create(struct infix_tree_node_list** list) {
  *list = malloc(sizeof(struct infix_tree_node_list));
  if ((*list) != NULL) {
    return simple_list_create(&(*list)->list);
  }
  return false;
}

bool tree_node_list_push(struct infix_tree_node_list* list, struct infix_tree_node* parentheses_node) {
  return simple_list_push(&list->list, parentheses_node);
}

bool tree_node_list_pop(struct infix_tree_node_list* list, struct infix_tree_node** parentheses_node) {
  return simple_list_pop(&list->list, (void**) parentheses_node);
}

enum infix_operator_node_type get_op_type(const char* token) {
  assert(strlen(token) == 1);
  switch(token[0]) {
    case '+':
      return infix_operator_node_add;
    case '-':
      return infix_operator_node_subtract;
    case '*':
      return infix_operator_node_multiply;
    case '/':
      return infix_operator_node_divide;
    case '^':
      return infix_operator_node_power;
  }
  assert(false);
}

long long str_to_ll(const char* string) {
  bool negative = false;
  if (*string == '-') {
    negative = true;
    ++string;
  }
  long long val = 0;
  while (*string != '\0') {
    int num = (*string) - '0';
    assert(num >= 0 && num <= 9);
    val *= 10;
    val += num;
    ++string;
  }
  if (negative) {
    val = -val;
  }
  return val;
}

struct infix_tree_node* new_blank_node(void) {
  struct infix_tree_node* node = malloc(sizeof(struct infix_tree_node));
  if (node != NULL) {
    node->type = infix_tree_node_blank;
    return node;
  } else {
    assert(false);
  }
}

bool is_left_to_right_operator(const enum infix_operator_node_type type) {
  switch (type) {
    case infix_operator_node_add:
    case infix_operator_node_subtract:
    case infix_operator_node_multiply:
    case infix_operator_node_divide:
      return true;
    case infix_operator_node_power:
      return false;
    default:
      assert(false);
  }
}

/* higher means higher precedence */
int get_op_precedence(const enum infix_operator_node_type type) {
  switch (type) {
    case infix_operator_node_power:
      return 2;
    case infix_operator_node_multiply:
    case infix_operator_node_divide:
      return 1;
    case infix_operator_node_add:
    case infix_operator_node_subtract:
      return 0;
    default:
      assert(false);
  }
}

void error_on_token(const char* token) {
  printf("Error parsing token: %s\n", token);
  exit(1);
}

void error_on_memory() {
  printf("Error processing input (out of memory)\n");
  exit(1);
}

/* TODO handle bool return values and malloc failure */
bool token_analyze(token_list_t* token_list, struct infix_tree_node** return_top_node) {
  struct infix_tree_node* top_node = NULL;
  /* affected by current parentheses */
  struct infix_tree_node_list* parentheses_list;
  if (!tree_node_list_create(&parentheses_list)) {
    error_on_memory();
  }
  /* NULL or a parentheses node */
  struct infix_tree_node* local_top_node_parent = NULL;
  struct infix_tree_node* local_top_node;
  struct infix_tree_node* new_node;

  top_node = new_blank_node();
  local_top_node = top_node;

  for (unsigned int i = 0; i < token_list->list.size; ++i) {
    struct infix_token* token = token_list->list.items[i];
    /* Handle numbers */
    if (token->type == infix_token_number) {
      new_node = malloc(sizeof(struct infix_tree_node));
      new_node->type = infix_tree_node_number;
      new_node->node.num.number = str_to_ll(token->token);

      struct infix_tree_node* compare_node = local_top_node;
      if (compare_node->type == infix_tree_node_operator) {
        while (true) {
          if (compare_node->node.op.right == NULL) {
            compare_node->node.op.right = new_node;
            break;
          }
          /* right is already guaranteed to be non-null */
          else if (compare_node->node.op.right->type == infix_tree_node_operator) {
            compare_node = compare_node->node.op.right;
            continue;
          } else {
            error_on_token(token->token);
          }
        }
      } else if (compare_node->type == infix_tree_node_blank) {
        if (local_top_node_parent != NULL) {
          local_top_node_parent->node.paren.child = new_node;
        } else {
          assert(top_node->type == infix_tree_node_blank);
          top_node = new_node;
        }
        local_top_node = new_node;
        free(compare_node);
      } else {
        error_on_token(token->token);
      }
    }
    /* Handle operators */
    else if (token->type == infix_token_operator) {
      new_node = malloc(sizeof(struct infix_tree_node));
      new_node->type = infix_tree_node_operator;
      new_node->node.op.type = get_op_type(token->token);
      new_node->node.op.left = NULL;
      new_node->node.op.right = NULL;

      int precedence = get_op_precedence(new_node->node.op.type);
      struct infix_tree_node* compare_node = local_top_node;
      struct infix_tree_node* compare_node_parent = local_top_node_parent;
      if (compare_node->type == infix_tree_node_operator) {
        while (true) {
          int compare_node_precedence = get_op_precedence(compare_node->node.op.type);
          if (compare_node_precedence < precedence) {
            if (compare_node->node.op.right != NULL) {
              if (compare_node->node.op.right->type == infix_tree_node_number || compare_node->node.op.right->type == infix_tree_node_parentheses) {
                new_node->node.op.left = compare_node->node.op.right;
                compare_node->node.op.right = new_node;
                break;
              } else if (compare_node->node.op.right->type == infix_tree_node_operator) {
                compare_node_parent = compare_node;
                compare_node = compare_node->node.op.right;
                continue;
              } else {
                error_on_token(token->token);
              }
            } else {
              error_on_token(token->token);
            }
          } else if (compare_node_precedence > precedence) {
            if (compare_node_parent != NULL) {
              if (compare_node_parent->type == infix_tree_node_operator) {
                compare_node_parent->node.op.right = new_node;
              } else if (compare_node_parent->type == infix_tree_node_parentheses) {
                compare_node_parent->node.paren.child = new_node;
              } else {
                assert(false);
              }
            } else {
              top_node = new_node;
              local_top_node = new_node;
            }
            new_node->node.op.left = compare_node;
            break;
          } else if (compare_node_precedence == precedence) {
            bool new_node_left_to_right = is_left_to_right_operator(new_node->node.op.type);
            if (new_node_left_to_right) {
              /* TODO fix copy paste code from just above */
              if (compare_node_parent != NULL) {
                if (compare_node_parent->type == infix_tree_node_operator) {
                  compare_node_parent->node.op.right = new_node;
                } else if (compare_node_parent->type == infix_tree_node_parentheses) {
                  compare_node_parent->node.paren.child = new_node;
                } else {
                  assert(false);
                }
              } else {
                top_node = new_node;
                local_top_node = new_node;
              }
              new_node->node.op.left = compare_node;
            } else {
              /* Node must be right to left */
              if (compare_node->node.op.right->type == infix_tree_node_number || compare_node->node.op.right->type == infix_tree_node_parentheses) {
                new_node->node.op.left = compare_node->node.op.right;
                compare_node->node.op.right = new_node;
              } else {
                error_on_token(token->token);
              }
            }
            break;
          }
        }
      } else if (compare_node->type == infix_tree_node_number || compare_node->type == infix_tree_node_parentheses) {
        if (compare_node_parent != NULL) {
          assert(compare_node_parent->type == infix_tree_node_parentheses);
          compare_node_parent->node.paren.child = new_node;
        } else {
          top_node = new_node;
        }
        local_top_node = new_node;
        new_node->node.op.left = compare_node;
      } else {
        error_on_token(token->token);
      }
    }
    /* Handle parentheses */
    else if (token->type == infix_token_parentheses) {
      assert(strlen(token->token) == 1);
      switch (token->token[0]) {
        case '(':
          {
            struct infix_tree_node* blank_node;
            blank_node = malloc(sizeof(struct infix_tree_node));
            blank_node->type = infix_tree_node_blank;

            new_node = malloc(sizeof(struct infix_tree_node));
            new_node->type = infix_tree_node_parentheses;
            new_node->node.paren.child = blank_node;

            struct infix_tree_node* compare_node = local_top_node;
            if (compare_node->type == infix_tree_node_operator) {
              while (true) {
                if (compare_node->node.op.right == NULL) {
                  compare_node->node.op.right = new_node;
                  break;
                }
                /* right is already guaranteed to be non-null */
                else if (compare_node->node.op.right->type == infix_tree_node_operator) {
                  compare_node = compare_node->node.op.right;
                  continue;
                } else {
                  error_on_token(token->token);
                }
              }
            } else if (compare_node->type == infix_tree_node_blank) {
              if (local_top_node_parent != NULL) {
                local_top_node_parent->node.paren.child = new_node;
              } else {
                top_node = new_node;
              }
              local_top_node = new_node;
              free(compare_node);
            } else {
              error_on_token(token->token);
            }
            bool status;
            if (local_top_node_parent != NULL) {
              status = tree_node_list_push(parentheses_list, local_top_node_parent);
            } else {
              /* The top_node doesn't have a parent to push */
              assert(local_top_node == top_node);
              status = tree_node_list_push(parentheses_list, top_node);
            }
            if (!status) {
              error_on_memory();
            }
            local_top_node_parent = new_node;
            local_top_node = blank_node;
            break;
          }
        case ')':
          {
            struct infix_tree_node* node_parent;
            bool status = tree_node_list_pop(parentheses_list, &node_parent);
            if (status) {
              assert(node_parent != NULL);
              /* The top_node may be parentheses which is why we need to check if the list is also not empty */
              if (node_parent->type == infix_tree_node_parentheses && parentheses_list->list.size != 0) {
                local_top_node = node_parent->node.paren.child;
                local_top_node_parent = node_parent;
              } else {
                /* It was the top_node which doesn't have a parentheses above it */
                local_top_node = node_parent;
                local_top_node_parent = NULL;
              }
            } else {
              /* Unbalanced parentheses */
              error_on_token(token->token);
            }
            break;
          }
        default:
          assert(false);
      }
    }
  }

  if (parentheses_list->list.size != 0) {
    printf("Error mismatched parentheses\n");
    return false;
  }
  tree_node_list_free(parentheses_list);

  *return_top_node = top_node;
  return true;
}
