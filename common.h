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
    const char* keyword;      // псевдоимя: "шестерни", "резьба", "дифференциал"
    command_func_t handler;   // указатель на функцию
};


// Упаковка double
struct maybe_double {
    bool valid;
    double value;
};
extern const struct maybe_double none_double;

#endif
