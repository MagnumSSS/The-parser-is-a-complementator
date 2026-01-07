#ifndef STACK_H
#define STACK_H

#include "common.h"

struct stack {
    size_t count;
    double* data;   // упрощаем: не нужна struct array_double
    size_t capacity;
};

struct stack stack_create(size_t size);
void stack_destroy(struct stack* s);
bool stack_push(struct stack* s, double value);
struct maybe_double stack_pop(struct stack* s);
bool stack_is_empty(const struct stack* s);
size_t stack_count(const struct stack* s);

#endif
