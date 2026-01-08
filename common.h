#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdint.h>   // ← здесь int8_t, size_t и т.д.
#include <stddef.h>

#define MAX_TOKENS 20
#define MAX_OPS 20


#define MAX_VARS 128
struct var {
    char name[64];
    double value;
};


// указатель на функции и структура
typedef void (*command_func_t)(void);
struct command {
    const char* keyword;      // псевдоимя функции: pri, var и т.д
    command_func_t handler;   // указатель на обработчик функции
};
extern const struct command commands[];
extern const size_t num_commands;

// Упаковка double
struct maybe_double {
    bool valid;
    double value;
};
extern const struct maybe_double none_double;

// Тип указателя на обработчик команды языка
typedef bool (*command_handler_t)(
    const char* line,
    struct var context[],
    size_t* context_len
);

#endif
