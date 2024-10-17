#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "string_parser.h"
#include "command.h"

int main(int argc, char *argv[]){
  char input[128];
  FILE *file;
  FILE *fp;
  int outputFile;

  if(argc > 1 && strcmp(argv[1], "-f") == 0){
    if (argc < 3) {
      printf("Error! No input file specified\n");  // Output to stdout
      return 1;
    }

    file = fopen(argv[2], "r");
    outputFile = open("output.txt", O_RDWR | O_CREAT | O_EXCL | O_APPEND, 0777);
    fp = freopen("output.txt", "w", stdout);

    if (file == NULL) {
      printf("Error! Unable to open the specified file\n");
      fclose(fp);
      return 1;
    }
  }

  while(1){
    if(argc > 1 && strcmp(argv[1], "-f") == 0){
      if (fgets(input, sizeof(input), file) == NULL) {
        break;
      }
    }else{
      write(1, ">>> ", strlen(">>> "));
      fgets(input , sizeof(input), stdin);
      if (strcmp(input, "exit\n") == 0) {
        break;
      }
    }
    command_line large_token;
    command_line small_token;

    large_token = str_filler(input, ";");

    for(int i = 0; large_token.command_list[i] != NULL; i++){
      small_token = str_filler(large_token.command_list[i], " ");
      if(small_token.command_list[0] == NULL){
        free_command_line(&small_token);
        continue;
      }

      if(strcmp(small_token.command_list[0], "ls") == 0){

        if(small_token.command_list[1]){ // Check if ls has a second parameter
          printf("Error! Unsupported parameters for command: ls\n");
        }else{
          listDir();
        }

      }else if(strcmp(small_token.command_list[0], "pwd") == 0){

        if(small_token.command_list[1]){
          printf("Error! Unsupported parameters for command: pwd\n");
        }else{
          showCurrentDir();
        }

      }else if(strcmp(small_token.command_list[0], "mkdir") == 0){

        if(small_token.command_list[1] != NULL){
          makeDir(small_token.command_list[1]);
        }else{
          printf("Error! No paramater specified for directory name\n");
        }

      }else if(strcmp(small_token.command_list[0], "cd") == 0){

        if(small_token.command_list[1] != NULL){
          changeDir(small_token.command_list[1]);
        }else{
          printf("Error! No paramater specified for directory change\n");
        }

      }else if(strcmp(small_token.command_list[0], "cp") == 0){

        if(small_token.command_list[1] != NULL && small_token.command_list[2] != NULL){
          copyFile(small_token.command_list[1], small_token.command_list[2]);
        }else{
          printf("Error! Needs both source and destination arguments\n");
        }

      }else if(strcmp(small_token.command_list[0], "mv") == 0){

        if(small_token.command_list[1] != NULL && small_token.command_list[2] != NULL){
          moveFile(small_token.command_list[1], small_token.command_list[2]);
        }else{
          printf("Error! Needs both source and destination arguments\n");
        }

      }else if(strcmp(small_token.command_list[0], "rm") == 0){

        if(small_token.command_list[1] != NULL){
          deleteFile(small_token.command_list[1]);
        }else{
          printf("Error! File cannot be deleted\n");
        }

      }else if(strcmp(small_token.command_list[0], "cat") == 0){

        if(small_token.command_list[1] != NULL){
          displayFile(small_token.command_list[1]);
        }else{
          printf("Error! File cannot be displayed\n");
        }

      }else{
        printf("Error! Unrecognized command: %s\n", small_token.command_list[0]);
      }
      free_command_line(&small_token);
    }
    free_command_line(&large_token);
  }
  if (argc > 1 && strcmp(argv[1], "-f") == 0) {
    fclose(file);
    fclose(fp);
    close(outputFile);
  }
  return 0;
}