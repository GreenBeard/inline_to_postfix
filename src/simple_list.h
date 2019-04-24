#ifndef SIMPLE_LIST_H
#define SIMPLE_LIST_H

#include <stdbool.h>

/* list of pointers */
struct doubling_list {
  void** items;
  long long size;
  long long capacity;
};

bool simple_list_push(struct doubling_list* list, void* item);
bool simple_list_pop(struct doubling_list* list, void** item);
bool simple_list_create(struct doubling_list* list);

#endif
