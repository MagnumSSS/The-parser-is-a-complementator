#ifndef PARSER_H
#define PARSER_H

#include "common.h"

bool is_operator(const char* token);
bool is_function(const char* token);
int8_t precedence(const char* op);

void tokenize(char* input, char* tokens[], size_t* count);
size_t infix_to_rpn(char* infix[], size_t infix_n, char* rpn_out[]);
struct maybe_double evaluate_rpn(char* tokens[], size_t n);

#endif
