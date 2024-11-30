#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

int main(int argc, char *argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *file = fopen(argv[1], "r");
  if(!file){
    perror("Error opening the file\n");
    return EXIT_FAILURE;
  }

  char buffer[256];
  int num_accounts;

  if(fgets(buffer, sizeof(buffer), file)){
    num_accounts = atoi(buffer);
  }else{
    fprintf(stderr, "Failed to get the number of accounts\n");
    fclose(file);
    return EXIT_FAILURE;
  }

  account *accounts = (account *)malloc(num_accounts * sizeof(account));

  if(!accounts){
    perror("Failed to allocate memory for the accounts\n");
    fclose(file);
    return EXIT_FAILURE;
  }



}