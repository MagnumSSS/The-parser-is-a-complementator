#ifndef FILE_IO_H
#define FILE_IO_H

#include "common.h"

#include <stdbool.h>
#include <stddef.h>


void clear_stdin();
bool find_formula(const char* filename, const char* formulaname, size_t* index);
void print_formula_by_index(const char* filename, size_t index, char* out, size_t out_size);
bool print_header(const char* filename, size_t* count);

#endif
