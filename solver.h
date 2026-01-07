// solver.h
#ifndef SOLVER_H
#define SOLVER_H

#include "common.h"
#include <stdbool.h>

// Объявляем функции
struct maybe_double compute_and_print(const char* formula, struct var context[], size_t* context_len);
struct maybe_double start_loop(char* name_file, char* name_formula, struct var context[], size_t* context_len);

// Объявляем clear_stdin (если её нет в utils.h)
void clear_stdin(void);

#endif // SOLVER_H
