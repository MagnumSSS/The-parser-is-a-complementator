#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <stdio.h>
#include <math.h>
#include "main.h"
#include "gears.h"
#include "threads.h"
#include "utils.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

//FILE* output_file = NULL;

static const struct command commands[] = {
    {"шестерни", gear_calc},
    {"косозубые_шестерни", bevel_gear_calc},
    {"планетарную", calculate_radial_distances},
		{"косозубую_планетарную", bevel_planetar_gear},
    {"коническую_передачу", calculate_bevel_gear},
    {"дифференциал", calculate_hypoid},
    {"межосевое_расстояние", calculate_center_distances},
    {"передаточное_число", gear_chain_ratio},
		{"профиль_резьбы", carving_profile},
		{"шаг_резьбы", find_word}
};

static const size_t num_commands = sizeof(commands) / sizeof(commands[0]);


int main() {
    //setup_log();
    main_menu(); // запуск главного меню
    /*
		if(output_file){
			fprintf(output_file, "\n==Конец Сессии ===\n");
      fclose(output_file);
    }
		*/
    return 0;
}

/*
void setup_log(){
        printf("Сохранить расчеты в файл(y/n)?\n");
        char choise;
        scanf(" %c", &choise);
        clear_stdin();

        if(choise == 'y' || choise == 'Y'){
                char filename[100];
                printf("Введите имя файла: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = '\0';

                output_file = fopen(filename, "a");
                if(output_file == NULL){
                        printf("Не удалось открыть файл '%s' \n", filename);
                        output_file = NULL;
                }
                else{
                        fprintf(output_file, "\n=== Сессия расчётов от %s ===\n", __DATE__);
                }

        }
}
*/

void print_functions(char* filename){
        FILE* file_with_functions = fopen(filename, "r");
        char buffer[256];
        if(file_with_functions){
          while(fgets(buffer, sizeof(buffer), file_with_functions)){
          //if(isspace(buffer[0])) continue;
          printf("%s", buffer);
          }
        }
        fclose(file_with_functions);
}

void parser(char** tokens, size_t count){
      if(count == 2 && strcmp(tokens[0], "рассчитать") == 0 ){
				for (size_t i = 0; i < num_commands; i++) {
					if (strcmp(commands[i].keyword, tokens[1]) == 0) {
						commands[i].handler();
						return;
					}
				}
      }
      else if(count == 3 && strcmp(tokens[0], "показать") == 0 && strcmp(tokens[1], "расчеты") == 0){
          if(strcmp(tokens[2], "шестерни") == 0 || strcmp(tokens[2], "шестерен") == 0){
            print_functions("gears");
            return;
					}
          else if(strcmp(tokens[2], "резьбы") == 0 || strcmp(tokens[2], "резьб") == 0){
            print_functions("threads");
            return;
          }
          else{
            printf("Для этой группы нет расчетов\n");
            return;
          }
        }else if(strcmp(tokens[0], "помощь") == 0){
					printf("Основные команды: \n");
					printf("показать расчеты шестерни/резьбы <- показывает все возможные расчеты той или иной группы\n");
					printf("рассчитать X <- рассчитывает одну из формул одной из групп\n");
					printf("выход <- выход из программы\n");
					return;
				}else{
            printf("Неправильно введена команда\n");
						return;
        }


}

// Реализация CLI
void main_menu() {
        bool run = true;
        printf("Введите команду \"помощь\" для информации о командах\n");
        while(run){
          printf("> ");
          char input[256];
          fgets(input, sizeof(input), stdin);
          input[strcspn(input, "\n")] = '\0';

          // Копия для strtok
          char buffer[256];
          strcpy(buffer, input);

          char* tokens[10];
          size_t count = 0;
          char* tok = strtok(buffer, " \t");
          while (tok && count < 10) {
              tokens[count++] = tok;
              tok = strtok(NULL, " \t");
          }
          if(count == 0){
               continue;
          }else if(count == 1 && strcmp(tokens[0], "выход") == 0){
               run = false;
          }else {
               parser(tokens, count);
          }
        }

}

