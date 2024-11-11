#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define TIME_SLICE 1 // Time quantum for the RR (Round Robin) algorithm

void alarm_handler(int sig);
void signaler(pid_t *pid_array, int size, int signal);
void display_process_info();

// Moved these outside of main so alarm_handler can access them
pid_t pid_array[10];
int num_processes = 0;
int current_process = 0;
int finished_processes = 0;
int process_completed[10] = {0}; // Array to track completed processes
int display_counter = 0; // For displaying the process info every full cycle

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

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGUSR1);
  sigprocmask(SIG_BLOCK, &sigset, NULL);
  signal(SIGALRM, alarm_handler);

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
      // printf("Child Process: %d - Waiting for SIGUSR1...\n", getpid());
      sigwait(&sigset, &sig);
      // printf("Child Process: %d - Received SIGUSR1, starting execution.\n", getpid());

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

  if (num_processes > 0) {
    printf("Scheduling Process %d\n", pid_array[current_process]);
    kill(pid_array[current_process], SIGCONT);
    alarm(TIME_SLICE);
  }

  for(int i = 0; i < num_processes; i++){
    if(!process_completed[i]){
      if(waitpid(pid_array[i], NULL, 0) < 0){
        perror("Waitpid failed");
      }else{
        process_completed[i] = 1;
      }
    }
  }

  return 0;
}

void display_process_info(){
  printf("\nPID\tutime\tstime\ttime\tnice\tvirt mem\n");

  long clock_ticks_per_sec = sysconf(_SC_CLK_TCK);

  for(int i = 0; i < num_processes; i++){
    if(process_completed[i]){
      continue;
    }

    char path[40];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid_array[i]);
    FILE *file = fopen(path, "r");

    if(file){
      long utime, stime;
      unsigned long vsize;
      int nice;
      float total_time;

      if(fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %d %*d %*d %*d %*d %*d %lu", &utime, &stime, &nice, &vsize) != 4){
        fprintf(stderr, "Error reading utime, stime, nice, and vsize for PID %d\n", pid_array[i]);
        fclose(file);
        exit(EXIT_FAILURE);
      }
      fclose(file);
      total_time = (float)(utime + stime) / clock_ticks_per_sec;

      printf("%d - %0.6f %0.6f %0.6f    %d  %lu\n",
        pid_array[i],
        (float)utime / clock_ticks_per_sec,
        (float)stime / clock_ticks_per_sec,
        total_time, nice, vsize);
    }else{
      fprintf(stderr, "Error opening /proc/%d/stat\n", pid_array[i]);
      exit(EXIT_FAILURE);
    }
  }
}



void alarm_handler(int sig){ // Round Robin implementation
  int status;
  if(waitpid(pid_array[current_process], &status, WNOHANG) > 0){
    if(WIFEXITED(status) || WIFSIGNALED(status)){
      finished_processes++;
      process_completed[current_process] = 1;
      if (finished_processes == num_processes) {
        printf("All child processes have completed.\n");
        exit(0);
      }
    }
  }else{
    kill(pid_array[current_process], SIGSTOP);
  }

  display_counter++;
  if(display_counter % 2 == 0){
    display_process_info();
  }

  current_process = (current_process + 1) % num_processes;

  int start = current_process;
  while(finished_processes < num_processes){
    if(waitpid(pid_array[current_process], &status, WNOHANG) == 0){
      //printf("Scheduling Process %d\n", pid_array[current_process]);
      kill(pid_array[current_process], SIGCONT);
      alarm(TIME_SLICE);
      break;
    }
    current_process = (current_process + 1) % num_processes;
    if(current_process == start)break;
  }
}

void signaler(pid_t *pid_array, int size, int signal){
  for(int i = 0; i < size; i++){
    // printf("Parent process: Sending signal %d to child process %d\n", signal, pid_array[i]);
    kill(pid_array[i], signal);
  }
}