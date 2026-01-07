#include "parser.h"
#include "stack.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// --- is_operator, is_function, precedence ---
bool is_operator(const char* token) {
    return token && (
        strcmp(token, "+") == 0 ||
        strcmp(token, "-") == 0 ||
        strcmp(token, "*") == 0 ||
        strcmp(token, "/") == 0
    );
}

bool is_function(const char* token) {
    return token && (
        strcmp(token, "sqrt") == 0 ||
        strcmp(token, "log") == 0 ||
        strcmp(token, "sin") == 0 ||
        strcmp(token, "cos") == 0 ||
        strcmp(token, "tan") == 0 ||
				strcmp(token, "atan") == 0 ||
				strcmp(token, "asin") == 0
    );
}

int8_t precedence(const char* op) {
    if (is_function(op)) return 3;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 0;
}

// --- tokenize ---
void tokenize(char* input, char* tokens[], size_t* count) {
    *count = 0;
    char* tok = strtok(input, " \t");
    while (tok && *count < MAX_TOKENS) {
        tokens[*count] = tok;
        (*count)++;
        tok = strtok(NULL, " \t");
    }
}

// --- infix_to_rpn ---
size_t infix_to_rpn(char* infix[], size_t infix_n, char* rpn_out[]) {
    size_t count = 0;
    char* op_stack[MAX_OPS];
    size_t op_count = 0;

    for (size_t i = 0; i < infix_n; i++) {
        if (strcmp(infix[i], "(") == 0) {
            op_stack[op_count++] = infix[i];
        }
        else if (strcmp(infix[i], ")") == 0) {
            while (op_count > 0 && strcmp(op_stack[op_count - 1], "(") != 0) {
                rpn_out[count++] = op_stack[--op_count];
            }
            if (op_count > 0) op_count--;
            if (op_count > 0 && is_function(op_stack[op_count - 1])) {
                rpn_out[count++] = op_stack[--op_count];
            }
        }
        else if (is_function(infix[i])) {
            op_stack[op_count++] = infix[i];
        }
        else if (is_operator(infix[i])) {
            int8_t curr_prec = precedence(infix[i]);
            while (op_count > 0 &&
                   strcmp(op_stack[op_count - 1], "(") != 0 &&
                   precedence(op_stack[op_count - 1]) >= curr_prec) {
                rpn_out[count++] = op_stack[--op_count];
            }
            op_stack[op_count++] = infix[i];
        }
        else {
            rpn_out[count++] = infix[i];
        }
    }

    while (op_count > 0) {
        rpn_out[count++] = op_stack[--op_count];
    }
    return count;
}

// --- evaluate_rpn ---
struct maybe_double evaluate_rpn(char* tokens[], size_t n) {
    struct stack s = stack_create(n);
    for (size_t i = 0; i < n; i++) {
        if (is_operator(tokens[i])) {
            struct maybe_double b = stack_pop(&s);
            struct maybe_double a = stack_pop(&s);
            if (!a.valid || !b.valid) { stack_destroy(&s); return none_double; }
            double res_val;
            if (strcmp(tokens[i], "+") == 0) res_val = a.value + b.value;
            else if (strcmp(tokens[i], "-") == 0) res_val = a.value - b.value;
            else if (strcmp(tokens[i], "*") == 0) res_val = a.value * b.value;
            else if (strcmp(tokens[i], "/") == 0) {
                if (b.value == 0) { stack_destroy(&s); return none_double; }
                res_val = a.value / b.value;
            } else { stack_destroy(&s); return none_double; }
            if (!stack_push(&s, res_val)) { stack_destroy(&s); return none_double; }
        }
        else if (is_function(tokens[i])) {
            struct maybe_double a = stack_pop(&s);
            if (!a.valid) { stack_destroy(&s); return none_double; }
            double res_val;
            if (strcmp(tokens[i], "sqrt") == 0) {
                if (a.value < 0) { stack_destroy(&s); return none_double; }
                res_val = sqrt(a.value);
            }
            else if (strcmp(tokens[i], "log") == 0) {
                if (a.value <= 0) { stack_destroy(&s); return none_double; }
                res_val = log(a.value);
            }
            else if (strcmp(tokens[i], "sin") == 0) res_val = sin(a.value);
            else if (strcmp(tokens[i], "cos") == 0) res_val = cos(a.value);
            else if (strcmp(tokens[i], "tan") == 0) res_val = tan(a.value);
						else if (strcmp(tokens[i], "atan") == 0) res_val = atan(a.value);
						else if (strcmp(tokens[i], "asin") == 0) res_val = asin(a.value);
            else { stack_destroy(&s); return none_double; }
            if (!stack_push(&s, res_val)) { stack_destroy(&s); return none_double; }
        }
        else {
            char* end;
            double val = strtod(tokens[i], &end);
            if (*end != '\0' || end == tokens[i]) { stack_destroy(&s); return none_double; }
            if (!stack_push(&s, val)) { stack_destroy(&s); return none_double; }
        }
    }

    if (stack_count(&s) == 1) {
        struct maybe_double result = stack_pop(&s);
        stack_destroy(&s);
        return result;
    }
    stack_destroy(&s);
    return none_double;
}
