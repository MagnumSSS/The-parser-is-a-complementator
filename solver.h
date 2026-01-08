// solver.h
#ifndef SOLVER_H
#define SOLVER_H

#include "common.h"
#include <stdbool.h>

// Объявляем функции
struct maybe_double compute_and_print(const char* formula, struct var context[], size_t* context_len);
bool run_script(const char* filename, struct var context[], size_t* context_len);
struct maybe_double is_assignment(const char* line, struct var context[], size_t* context_len);
struct maybe_double handle_pri(const char* line, struct var context[], size_t* context_len);
struct maybe_double interpret_line(const char* line, struct var context[], size_t* context_len);


// Объявляем clear_stdin (если её нет в utils.h)
void clear_stdin(void);

#endif // SOLVER_H
