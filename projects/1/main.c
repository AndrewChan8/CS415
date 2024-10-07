#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

int main(int argc, char *argv[]){
  char *line = NULL;
  size_t len = 0;
  char input[128];
  command_line cmd;

  if(argc > 1 && strcmp(argv[1], "-f") == 0){
    printf("File mode\n");
  }else{
    printf("Interactive mode. Type 'exit' to exit.\n");
    while(1){
      printf(">>> ");
      fgets(input, sizeof(input), stdin);
      input[strcspn(input, "\n")] = '\0'; // Remove the \n character at the location that it occurs
      
      if(strcmp(input, "exit") == 0){
        break;
      }else{
        printf("%s\n", input);
      }
    }
  }
}