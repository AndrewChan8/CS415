#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void signaler(pid_t *pid_array, int size, int signal);

int main(int argc, char *argv[]){
  if(argc != 3){
    fprintf(stderr, "Invalid use: incorrect number of parameters\n");
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "-f") != 0) {
    fprintf(stderr, "Error: Missing '-f' flag\n");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(argv[2], "r");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  int max_args = 10;
  char line[1024];
  pid_t pid_array[max_args];
  int num_processes = 0;

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGUSR1);
  sigprocmask(SIG_BLOCK, &sigset, NULL);

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
      perror("Failed to fork process");
      fclose(file);
      exit(EXIT_FAILURE);
    }else if(pid == 0){
      fclose(file);
      int sig;
      printf("Child Process: %d - Waiting for SIGUSR1...\n", getpid());

      sigwait(&sigset, &sig);
      printf("Child Process: %d - Received SIGUSR1, starting execution.\n", getpid());

      if(execvp(args[0], args) == -1) {
        perror("Execvp failed");
        exit(EXIT_FAILURE);
      }
    }else{
      pid_array[num_processes++] = pid;
    }
  }
  fclose(file);

  signaler(pid_array, num_processes, SIGUSR1);

  signaler(pid_array, num_processes, SIGSTOP);

  for (int i = 0; i < num_processes; i++) {
    printf("Resuming process %d\n", pid_array[i]);
    kill(pid_array[i], SIGCONT);
    sleep(1);  // Delay to simulate time-sliced scheduling
  }

  for (int i = 0; i < num_processes; i++) {
    if (waitpid(pid_array[i], NULL, 0) < 0) {
      perror("Waitpid failed");
    }
  }

  return 0;
}

void signaler(pid_t *pid_array, int size, int signal) {
  sleep(3);
  for (int i = 0; i < size; i++) {
    printf("Parent process: Sending signal %d to child process %d\n", signal, pid_array[i]);
    kill(pid_array[i], signal);
  }
}