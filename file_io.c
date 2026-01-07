#include "file_io.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



void print_formula_by_index(const char* filename, size_t index, char* out, size_t out_size) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        if (out) out[0] = '\0';
        return;
    }

    char buffer[256];
    size_t current_index = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '#') {
            if (current_index == index) {
                if (fgets(buffer, sizeof(buffer), fp)) {
                    buffer[strcspn(buffer, "\n")] = '\0';
                    strncpy(out, buffer, out_size - 1);
                    out[out_size - 1] = '\0';
                } else {
                    out[0] = '\0';
                }
                break;
            }
            current_index++;
        }
    }
    fclose(fp);
}

bool find_formula(const char* filename, const char* formulaname, size_t* index){
	FILE* fp = fopen(filename, "r");
	if(!fp){
		printf("Не удалось найти файл\n");
		return false;
	}
	char buffer[256];
	while(fgets(buffer, sizeof(buffer), fp)){
		buffer[strcspn(buffer, "\n")] = '\0';
		if(buffer[0] == '#'){
			if(strcmp(buffer+1, formulaname) == 0){
				fclose(fp);
				return true;
			}
		(*index)++;
		}
	}
	fclose(fp);
	return false;
}


bool find_param(const char* filename, size_t index, char* out) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Не удалось найти файл\n");
        if (out) out[0] = '\0';
        return false;
    }

    char buffer[256];
    size_t count = 0;
    bool found = false;
    char header_line[256] = {0};

    // Ищем заголовок с нужным индексом
    while (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '#') {
            if (count == index) {
                strcpy(header_line, buffer);
                found = true;
                break;
            }
            count++;
        }
    }
    fclose(fp);

    if (!found) {
        if (out) out[0] = '\0';
        return false;
    }

    // Ищем последнее '-' в строке
    char* dash = strrchr(header_line, '-');
    if (dash && *(dash + 1) != '\0') {
        // Копируем всё после '-'
        strcpy(out, dash + 1);
        // Убираем возможный \r или \n (на всякий случай)
        out[strcspn(out, "\r\n")] = '\0';
        return true;
    } else {
        // Нет '-' или имя пустое
        if (out) out[0] = '\0';
        return false;
    }
}

bool print_header(const char* filename, size_t* count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Не удалось найти файл\n");
        return false;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '#') {
            printf("%zu. %s\n", *count, buffer);
            (*count)++;
        }
    }
    fclose(fp);
    return true;
}
