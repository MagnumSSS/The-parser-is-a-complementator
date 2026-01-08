#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "solver.h"

int main() {
		struct var global_context[MAX_VARS];
		size_t context_len = 0;

		if (!run_script("test.dsl", global_context, &context_len)) {
			printf("Скрипт завершился с ошибкой.\n");
		}
    return 0;
}

