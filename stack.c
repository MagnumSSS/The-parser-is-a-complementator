#include "stack.h"
#include <stdlib.h>

const struct maybe_double none_double = {.valid = false, .value = 0.0};

struct maybe_double some_double(double i) {
    return (struct maybe_double){.value = i, .valid = true};
}

struct stack stack_create(size_t size) {
    struct stack s = {0};
    s.data = malloc(sizeof(double) * size);
    s.capacity = size;
    return s;
}

void stack_destroy(struct stack* s) {
    if (s && s->data) {
        free(s->data);
        s->data = NULL;
        s->count = 0;
        s->capacity = 0;
    }
}

bool stack_push(struct stack* s, double value) {
    if (!s || s->count >= s->capacity) return false;
    s->data[s->count++] = value;
    return true;
}

struct maybe_double stack_pop(struct stack* s) {
    if (!s || s->count == 0) return none_double;
    return some_double(s->data[--s->count]);
}

bool stack_is_empty(const struct stack* s) {
    return s && s->count == 0;
}

size_t stack_count(const struct stack* s) {
    return s ? s->count : 0;
}
