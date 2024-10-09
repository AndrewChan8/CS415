#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"
#include "command.h"

int main(int argc, char *argv[]){
  char *line = NULL;
  size_t len = 0;
  char input[128];
  command_line large_token;
  command_line small_token;

  if(argc > 1 && strcmp(argv[1], "-f") == 0){
    printf("File mode\n");
  }else{
    printf("Interactive mode. Type 'exit' to exit.\n");
    while(1){
      printf(">>> ");
      fgets(input, sizeof(input), stdin);
      input[strcspn(input, "\n")] = '\0'; // Replace the \n character at the location that it occurs

      if(strcmp(input, "exit") == 0){
        break;
      }else{
        large_token = str_filler(input, ";");
        for(int i = 0; large_token.command_list[i] != NULL; i++){
          small_token = str_filler(large_token.command_list[i], " ");
          for(int j = 0; small_token.command_list[j] != NULL; j++){
            if(strcmp(small_token.command_list[j], "ls") == 0){         // ls
              if(small_token.command_list[j+1]){ // Check if ls has a second parameter
                printf("Error! Unsupported parameters for command: ls\n");
                break;
              }else{
                listDir();
              }
            }else if(strcmp(small_token.command_list[j], "pwd") == 0){  // pwd
              if(small_token.command_list[j+1]){ // Check if ls has a second parameter
                printf("Error! Unsupported parameters for command: pwd\n");
                break;
              }else{
                showCurrentDir();
              }
            }else if(strcmp(small_token.command_list[j], "mkdir") == 0){
              if(small_token.command_list[j+1] != NULL){ // Check if ls has a second parameter
                makeDir(small_token.command_list[j+1]);
                break; // Prevent dir name from being passed in as next command
              }else{
                printf("Error! No paramater specified for directory name\n");
                break;
              }
            }else{
              printf("Error! Unrecognized command: %s\n", small_token.command_list[j]);
            }
          }
          free_command_line(&small_token);
        }
        free_command_line(&large_token);
      }
    }
  }
  return 0;
}