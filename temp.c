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

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



