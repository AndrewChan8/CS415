#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "account.h"

#define NUM_WORKERS 10

void process_transaction(account *accounts, int num_accounts, const char *transaction);
void update_balance();
void *process_worker(void *arg);

int num_accounts;
account *accounts;
pthread_mutex_t account_mutex;

pthread_t threads[NUM_WORKERS];
char **transactions;

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

  FILE *output_file = fopen("output.txt", "w");
  if(!output_file){
    perror("Error creating the output file");
    fclose(file);
    return EXIT_FAILURE;
  }

  char buffer[256];

  if(fgets(buffer, sizeof(buffer), file)){
    num_accounts = atoi(buffer);
  }else{
    fprintf(stderr, "Failed to get the number of accounts\n");
    fclose(file);
    return EXIT_FAILURE;
  }

  accounts = (account *)malloc(num_accounts * sizeof(account));
  if(!accounts){
    perror("Failed to allocate memory for the accounts\n");
    fclose(file);
    return EXIT_FAILURE;
  }

  pthread_mutex_init(&account_mutex, NULL);

  // Accounts
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
      buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline from buffer
      strncpy(accounts[i].account_number, buffer, sizeof(accounts[i].account_number) - 1);
      accounts[i].account_number[sizeof(accounts[i].account_number) - 1] = '\0';
    }else{
      fprintf(stderr, "Error reading account number for account %d\n", i);
      free(accounts);
      fclose(file);
      return EXIT_FAILURE;
    }

    // Password
    if(fgets(buffer, sizeof(buffer), file)){
      buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline from buffer
      strncpy(accounts[i].password, buffer, sizeof(accounts[i].password) - 1);
      accounts[i].password[sizeof(accounts[i].password) - 1] = '\0';
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

  // Count transactions
  int total_transactions = 0;
  while (fgets(buffer, sizeof(buffer), file)) {
    total_transactions++;
  }
  rewind(file);

  // Skip account setup lines again
  fgets(buffer, sizeof(buffer), file);
  for (int i = 0; i < num_accounts * 5; i++) {
    fgets(buffer, sizeof(buffer), file);
  }

  // Allocate memory for transactions
  transactions = malloc(total_transactions * sizeof(char *));
  if (!transactions) {
    perror("Failed to allocate memory for transactions");
    return EXIT_FAILURE;
  }

  // Store transactions
  int index = 0;
  while (fgets(buffer, sizeof(buffer), file)) {
    transactions[index] = strdup(buffer); // Duplicate transaction line
    index++;
  }

  // Now, divide transactions and create worker threads
  int transactions_per_thread = total_transactions / NUM_WORKERS;
  int remaining_transactions = total_transactions % NUM_WORKERS;

  for (int i = 0; i < NUM_WORKERS; i++) {
    int start_index = i * transactions_per_thread;
    int end_index = start_index + transactions_per_thread;

    if (i == NUM_WORKERS - 1) {
        end_index += remaining_transactions;
    }

    // Pass start and end indices as a struct
    int *range = malloc(2 * sizeof(int));
    range[0] = start_index;
    range[1] = end_index;

    pthread_create(&threads[i], NULL, process_worker, (void *)range);
  }

  // Wait for all threads to finish
  for (int i = 0; i < NUM_WORKERS; i++) {
    pthread_join(threads[i], NULL);
  }

  update_balance();

  for (int i = 0; i < num_accounts; i++) {
    fprintf(output_file, "%d balance:\t%.2f\n\n", i, accounts[i].balance);
    // printf("%d balance:\t%.2f\n\n", i, accounts[i].balance);
  }

  // Free memory
  for (int i = 0; i < total_transactions; i++) {
    free(transactions[i]);
  }

  free(transactions);
  free(accounts);
  pthread_mutex_destroy(&account_mutex);

  fclose(file);
  fclose(output_file);
}

void process_transaction(account *accounts, int num_accounts, const char *transaction){
  char transaction_type;
  char account_number[17];
  char password[9];
  double amount;
  char dest_account_number[17];

  if(transaction[0] == 'D'){ // Deposit
    sscanf(transaction, "D %s %s %lf", account_number, password, &amount);
    if(amount <= 0){
      fprintf(stderr, "Invalid deposit amount: %s", transaction);
      return;
    }
    for(int i = 0; i < num_accounts; i++){
      if(strcmp(accounts[i].account_number, account_number) == 0 && strcmp(accounts[i].password, password) == 0){
        accounts[i].balance += amount;
        accounts[i].transaction_tracter += amount;
        return;
      }
    }
  }else if(transaction[0] == 'W'){ // Withdrawl
    sscanf(transaction, "W %s %s %lf", account_number, password, &amount);
    if(amount <= 0){
      fprintf(stderr, "Invalid withdrawal amount: %s", transaction);
      return;
    }
    for(int i = 0; i < num_accounts; i++){
      if(strcmp(accounts[i].account_number, account_number) == 0 && strcmp(accounts[i].password, password) == 0){
        if(accounts[i].balance >= amount){
          accounts[i].balance -= amount;
          accounts[i].transaction_tracter += amount;
        }
        return;
      }
    }
  }else if(transaction[0] == 'T'){ // Transfer
    sscanf(transaction, "T %s %s %s %lf", account_number, password, dest_account_number, &amount);
    if(amount <= 0){
      fprintf(stderr, "Invalid transfer amount: %s", transaction);
      return;
    }
    int source_index = -1, destination_index = -1;
    for(int i = 0; i < num_accounts; i++){
      if(strcmp(accounts[i].account_number, account_number) == 0 && strcmp(accounts[i].password, password) == 0){
        source_index = i;
      }
      if(strcmp(accounts[i].account_number, dest_account_number) == 0){
        destination_index = i;
      }
    }
    if(source_index != -1 && destination_index != -1){
      if(accounts[source_index].balance >= amount){
        accounts[source_index].balance -= amount;
        accounts[destination_index].balance += amount;
        accounts[source_index].transaction_tracter += amount;
      }
    }
  }else if(transaction[0] == 'C'){ // Check
    sscanf(transaction, "C %s %s", account_number, password);
    for(int i = 0; i < num_accounts; i++){
      if (strcmp(accounts[i].account_number, account_number) == 0 && strcmp(accounts[i].password, password) == 0) {
        return;
      }
    }
  }else{ // Invalid
    fprintf(stderr, "Unknown transaction type: %s", transaction);
  }
}

void update_balance(){
  for(int i = 0; i < num_accounts; i++){
    accounts[i].balance += accounts[i].transaction_tracter * accounts[i].reward_rate;
  }
}

void *process_worker(void *arg) {
  int *range = (int *)arg;
  int start_index = range[0];
  int end_index = range[1];
  free(range);

  for (int i = start_index; i < end_index; i++) {
    pthread_mutex_lock(&account_mutex);
    process_transaction(accounts, num_accounts, transactions[i]);
    pthread_mutex_unlock(&account_mutex);
  }

  return NULL;
}