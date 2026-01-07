#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "parser.h"
#include "file_io.h"
#include "utils.h"

struct maybe_double compute_and_print(const char* formula, struct var context[], size_t* context_len) {
    if (!formula || formula[0] == '\0') {
        printf("Пустая формула\n");
        return none_double;
    }

    char input[1024];
    strncpy(input, formula, sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    char* tokens[MAX_TOKENS];
    size_t count = 0;
    tokenize(input, tokens, &count);

    char* rpn_out[MAX_TOKENS];
    size_t rpn_n = infix_to_rpn(tokens, count, rpn_out);

    char number_strs[MAX_TOKENS][64];
    char* final_tokens[MAX_TOKENS];

    //Обрабатываем каждый токен в RPN
    for (size_t i = 0; i < rpn_n; i++) {
        char* tok = rpn_out[i];

        if (is_operator(tok)) {
            final_tokens[i] = tok;
        }
        else if (is_function(tok)) {
            final_tokens[i] = tok;
        }
        else {
            // Проверяем, является ли токен числом
            char* end;
            strtod(tok, &end);
            if (*end == '\0' && end != tok) {
                final_tokens[i] = tok;
            }
            else {
                // Это переменная — ищем в контексте
                double known_value = 0.0;
                bool found = false;
                for (size_t j = 0; j < *context_len; j++) {
                    if (strcmp(context[j].name, tok) == 0) {
                        known_value = context[j].value;
                        found = true;
                        break;
                    }
                }

                if (found) {
                    snprintf(number_strs[i], sizeof(number_strs[i]), "%g", known_value);
                    final_tokens[i] = number_strs[i];
                } else {
                    double val;
                    printf("Введите значение переменной %s: ", tok);
                    scanf("%lf", &val);
                    clear_stdin();

                    // Сохраняем в контекст
                    if (*context_len < MAX_VARS) {
                        strncpy(context[*context_len].name, tok, sizeof(context[0].name) - 1);
                        context[*context_len].name[sizeof(context[0].name) - 1] = '\0';
                        context[*context_len].value = val;
                        (*context_len)++;
                    }

                    snprintf(number_strs[i], sizeof(number_strs[i]), "%g", val);
                    final_tokens[i] = number_strs[i];
                }
            }
        }
    }

    struct maybe_double result = evaluate_rpn(final_tokens, rpn_n);
    if (!result.valid) {
        printf("None\n");
    }
    return result;
}

struct maybe_double start_loop(char* name_file, char* name_formula, struct var context[], size_t* context_len){
		if(name_formula){
			size_t index = 0;
			if(find_formula(name_file, name_formula, &index)){
				char formula[1024];
				print_formula_by_index(name_file, index, formula, sizeof(formula));
				if (formula[0] == '\0') {
					printf("Формула не найдена\n");
					return none_double;
				}
				struct maybe_double res= compute_and_print(formula, context, context_len);
				if(res.valid){
					if(*context_len < MAX_VARS && name_formula){
						strncpy(context[*context_len].name, name_formula, sizeof(context[0].name) - 1);
						context[*context_len].name[sizeof(context[0].name) - 1] = '\0';
						context[*context_len].value = res.value;
						(*context_len)++;
					}
				}
				return res;
			}	
		}
		return none_double;
}

struct maybe_double is_assignment(const char* line, struct var context[], size_t* context_len){
	if(!line || !context || !context_len){
		return none_double;
	}
	// находим позицию '='
	const char* eq = strchr(line, '=');
	if(eq){
		// 1 часть - выделяем имя переменной

		char var_name[64];
		// выделяем левую часть - eq - line это расстояние от имени до '='
		size_t lhs_len = (size_t)(eq - line);
		if(lhs_len == 0 || l1_len >= sizeof(var_name)){
			printf("Ошибка: Пустое или Большое(макс. 64 символа) имя переменной");
			return none_double;
		}

		// копируем имя в массив
		strncpy(var_name, line, lhs_len);
		// завершаем нулем
		var_name[lhs_len] = '\0';

		// но если вдруг от имени до '=' много пробелов, убираем их
		while (lhs_len > 0 && (var_name[lhs_len - 1] == ' ' || var_name[lhs_len - 1] == '\t')) {
			var_name[--lhs_len] = '\0';
		}

		// 2 часть - выделяем выражение

		// все что после '=' наше выражение
		const char* expr = eq + 1; 

		// убираем пробелы начальные
		while(*expr == ' ' || *expr == '\t') expr++;

		if (*expr == '\0') {
      printf("Ошибка: пустое выражение после '='\n");
      return none_double;
    }

		// 3 часть - отдаем выражение калькулятору

		// вычисляем выражение
    struct maybe_double result = compute_and_print(expr, context, context_len);
    if (!result.valid) {
      printf("Ошибка вычисления выражения: %s\n", expr);
      return none_double;
    }

		// cохраняем в контекст
    bool found = false;
    for (size_t i = 0; i < *context_len; i++) {
      if (strcmp(context[i].name, var_name) == 0) {
				context[i].value = result.value;
        found = true;
        break;
      }
		}
		if (!found) {
      if (*context_len < MAX_VARS) {
        strncpy(context[*context_len].name, var_name, sizeof(context[0].name) - 1);
        context[*context_len].name[sizeof(context[0].name) - 1] = '\0';
        context[*context_len].value = result.value;
        (*context_len)++;
      }else {
        printf("Контекст переполнен\n");
        return none_double;
      }
    }
		return result;
	}


}

struct maybe_double interpret_line(const char* line, struct var context[], size_t* context_len){
	if(!line || !context || !context_len){
		return none_double;
	}
	
	// находит символ в строке
	if(strchr(line, '=')){
		return is_assignment(line, context, &context_len);
	}

}
