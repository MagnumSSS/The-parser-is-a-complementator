#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "parser.h"
#include "file_io.h"
#include "solver.h"
#include "stack.h"

// запуск парсера
bool run_script(const char* filename, struct var context[], size_t* context_len) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Не могу открыть файл: %s\n", filename);
        return false;
    }

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        // Убираем символ новой строки
        line[strcspn(line, "\n")] = '\0';

        // Пропускаем пустые строки и комментарии
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        // Обрабатываем строку
				struct maybe_double result = interpret_line(line, context, context_len);
        if (!result.valid) {
            printf("Ошибка в строке %d: %s\n", line_num, line);
            fclose(fp);
            return false;
        }
    }

    fclose(fp);
    return true;
}



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
		if(lhs_len == 0 || lhs_len >= sizeof(var_name)){
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

	return none_double;
}

struct maybe_double handle_pri(const char* line, struct var context[], size_t* context_len){
	// извлекаем переменную между скобками
	const char* open = strchr(line, '(');
	const char* close = strchr(line, ')');
	if(!open || !close || open >= close){
		printf("Ошибка: pri()\n");
		return none_double;
	}
	// проверяем
	char name[64];
	size_t len = (size_t)(close - open - 1);
	if(len >= sizeof(name)){
		printf("Ошибка: слишком длинная строка(макс. 64 символа)\n");
		return none_double;
	}
	// копируем
	strncpy(name, open + 1, len);
	name[len] = '\0';
	
	// убираем пробелы по краям
	//указатель на первый символ
  char* start = name;
	// пропускаем начальные пробелы
  while (*start == ' ' || *start == '\t') start++;
	// получаем указатель на последний символ
  char* end = start + strlen(start);
	// меняем пробелы на нуль символ
  while (end > start && (*(end - 1) == ' ' || *(end - 1) == '\t'))
      *--end = '\0';

  memmove(name, start, end - start);
  name[end - start] = '\0';

	// ищем в контексте
	for(size_t i = 0; i < *context_len; i++){
		if(strcmp(name, context[i].name) == 0){
			printf("%.6g\n", context[i].value);
			return some_double(context[i].value);
		}
	}

	printf("Переменная '%s' не найдена\n", name);
	return none_double;

}

struct maybe_double interpret_line(const char* line, struct var context[], size_t* context_len){
	if(!line || !context || !context_len){
		return none_double;
	}
	
	// находит символ в строке
	if(strchr(line, '=')){
		return is_assignment(line, context, context_len);
	}
	// 2. Команды: проверяем начало строки
  if (strncmp(line, "pri(", 4) == 0) {
    return handle_pri(line, context, context_len);
  }

	// иначе если это ни то и ни другое
	return compute_and_print(line, context, context_len);
}
