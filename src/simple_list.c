#include <stdbool.h>
#include <stdlib.h>

#include "./simple_list.h"

bool simple_list_push(struct doubling_list* list, void* item) {
  if (list->size < list->capacity) {
    list->items[list->size] = item;
    (list->size)++;
    return true;
  } else {
    void** new_items = realloc(list->items, sizeof(void*) * list->capacity * 2);
    if (new_items != NULL) {
      list->items = new_items;
      list->capacity *= 2;
      list->items[list->size] = item;
      (list->size)++;
      return true;
    } else {
      return false;
    }
  }
}

bool simple_list_pop(struct doubling_list* list, void** item) {
  if (list->size >= 1) {
    (list->size)--;
    *item = list->items[list->size];
    return true;
  } else {
    return false;
  }
}

bool simple_list_create(struct doubling_list* list) {
  list->capacity = 8;
  list->items = malloc(sizeof(void*) * list->capacity);
  list->size = 0;
  return (list->items != NULL);
}
