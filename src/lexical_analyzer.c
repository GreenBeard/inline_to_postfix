#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./lexical_analyzer.h"

#define is_digit(c) isdigit(c)
bool is_operator(char c) {
  switch (c) {
    case '+':
    case '-':
    case '/':
    case '*':
    case '^':
      return true;
    default:
      return false;
  }
}

void token_list_new(token_list_t* token_list) {
  simple_list_create(&token_list->list);
  /* FIX ME (handle failure) */
}

void token_clear(struct infix_token* token) {
  token->type = infix_token_none;
  token->token = NULL;
  token->token_len_cap = 0;
}

/* Pushes the token to the list and clears the token.
 * If the token is of type "none" then it only clears it.
 */
bool token_push_and_clear(token_list_t* token_list, struct infix_token* const token) {
  if (token->type != infix_token_none) {
    struct infix_token* token_copy = malloc(sizeof(struct infix_token));
    if (token_list != NULL) {
      memcpy(token_copy, token, sizeof(struct infix_token));
      bool success = simple_list_push(&token_list->list, token_copy);
      if (!success) {
        return false;
      }
    } else {
      return false;
    }
  }
  token_clear(token);
  return true;
}

/* Adds a character to the given token struct's token.
 * Returns true on success, false on failure.
 */
bool token_push_char(struct infix_token* token, char c) {
  size_t current_length;
  if (token->token != NULL) {
    current_length = strlen(token->token);
  } else {
    current_length = 0;
  }
  if (token->token_len_cap < current_length + 1) {
    /* Most tokens are short so allocate sparingly */
    char* new_token = realloc(token->token, sizeof(char) * (current_length + 4));
    if (new_token == NULL) {
      return false;
    } else {
      token->token = new_token;
      token->token_len_cap = current_length + 4;
    }
  }

  token->token[current_length] = c;
  token->token[current_length + 1] = '\0';
  return true;
}

bool parse_token(char c, struct infix_token* const token, token_list_t* token_list) {
  if (is_digit(c)) {
    if (token->type != infix_token_number) {
      token_push_and_clear(token_list, token);
      token->type = infix_token_number;
    }
    token_push_char(token, c);
  } else if (is_operator(c)) {
    /* '-' following a operator means a negative or if it was the *first* character */
    if (c == '-' && (token->type == infix_token_operator || token->type == infix_token_none)) {
      token_push_and_clear(token_list, token);
      token->type = infix_token_number;
      token_push_char(token, c);
    } else {
      token_push_and_clear(token_list, token);
      token->type = infix_token_operator;
      token_push_char(token, c);
    }
  } else if (c == '(' || c == ')') {
    token_push_and_clear(token_list, token);
    token->type = infix_token_parentheses;
    token_push_char(token, c);
  } else if (c == ' ' || c == '\t') {
    /* do nothing */
  } else {
    /* unrecognized character */
    return false;
  }
  return true;
}

bool lexical_analyze(FILE* file, token_list_t* token_list) {
  bool success = true;

  /* Finite state machine controls */
  struct infix_token token;
  token_clear(&token);
  token_list_new(token_list);

  while (true) {
    char c = getc(stdin);
    if (c != EOF && c != '\0' && c != '\n') {
      if (!parse_token(c, &token, token_list)) {
        printf("Error parsing character %c\n", c);
        success = false;
        break;
      }
    } else {
      token_push_and_clear(token_list, &token);
      break;
    }
  }

  return success;
}
