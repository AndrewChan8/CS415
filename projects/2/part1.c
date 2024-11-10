#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

  if(argc != 3){
    printf("Invalid use: incorrect number of parameters\n");
    exit(1);
  }

  if (strcmp(argv[1], "-f") != 0) {
    printf("Error: Missing '-f' flag\n");
    exit(1);
  }

  FILE *file = fopen(argv[2], "r");
  if (!file) {
    printf("Error opening file\n");
    exit(1);
  }

  int max_args = 10;
  char line[1024];
  pid_t pid_array[max_args];
  int i = 0;

  while(fgets(line, sizeof(line), file) != NULL){
    line[strcspn(line, "\n")] = '\0';

    char *args[max_args];
    char *token = strtok(line, " ");
    int j = 0;

    while (token != NULL && j < max_args) {
      args[j++] = token;
      token = strtok(NULL, " ");
    }
    args[j] = NULL; // Null terminate for execvp

    pid_t pid = fork();
    if(pid < 0){
      printf("Failed to allocate memory for pid array\n");
      fclose(file);
      exit(1);
    }else if(pid == 0){
      fclose(file);
      if(execvp(args[0], args) == -1) {
        printf("Execvp failed for command '%s'\n", args[0]);
        exit(1);
      }
    }else{
      pid_array[i++] = pid;
    }
  }
  fclose(file);
  for (int k = 0; k < i; k++) {
    if (waitpid(pid_array[k], NULL, 0) < 0) {
      printf("Waitpid failed\n");
    }
  }

  return 0;
}