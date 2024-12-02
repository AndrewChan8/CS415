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

  for(int i = 0; i < num_accounts; i++){

    // Index line
    if(!fgets(buffer, sizeof(buffer), file)){
      fprintf(stderr, "Error reading index line for account %d\n", i);
      free(accounts);
      fclose(file);
      return EXIT_FAILURE;
    }

    // Account Number
    if(fgets(buffer, sizeof(buffer), file)){
      strncpy(accounts[i].account_number, buffer, sizeof(accounts[i].account_number) - 1);
      accounts[i].account_number[strcspn(accounts[i].account_number, "\n")] = '\0'; // Remove newline
    }else{
      fprintf(stderr, "Error reading account number for account %d\n", i);
      free(accounts);
      fclose(file);
      return EXIT_FAILURE;
    }

    // Password
    if(fgets(buffer, sizeof(buffer), file)){
      strncpy(accounts[i].password, buffer, sizeof(accounts[i].password) - 1);
      accounts[i].password[strcspn(accounts[i].password, "\n")] = '\0'; // Remove newline
    }else{
      fprintf(stderr, "Error reading password for account %d\n", i);
      free(accounts);
      fclose(file);
      return EXIT_FAILURE;
    }

    // Balance
    if(fgets(buffer, sizeof(buffer), file)){
      accounts[i].balance = atof(buffer);
    }else{
      fprintf(stderr, "Error reading balance for account %d\n", i);
      free(accounts);
      fclose(file);
      return EXIT_FAILURE;
    }

    // Reward rate
    if(fgets(buffer, sizeof(buffer), file)){
      accounts[i].reward_rate = atof(buffer);
    }else{
      fprintf(stderr, "Error reading reward rate for account %d\n", i);
      free(accounts);
      fclose(file);
      return EXIT_FAILURE;
    }

    // transaction_tracter
    accounts[i].transaction_tracter = 0.0;

    // out_file
    snprintf(accounts[i].out_file, sizeof(accounts[i].out_file), "act_%d.txt", i);
  }

  for (int i = 0; i < num_accounts; i++) {
    printf("Account %d:\n", i);
    printf("  Number: %s\n", accounts[i].account_number);
    printf("  Password: %s\n", accounts[i].password);
    printf("  Balance: %.2f\n", accounts[i].balance);
    printf("  Reward Rate: %.3f\n", accounts[i].reward_rate);
  }

  free(accounts);
  fclose(file);
}