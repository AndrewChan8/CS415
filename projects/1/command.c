#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"

void write_message(char *message){// Helper function so I don't have to make a char[] everytime I want to write a message
  write(1, message, strlen(message));
}

void listDir(){
  DIR *directory;
  struct dirent *entry;

  directory = opendir(".");
  if(directory == NULL){
    char error[] = "Unable to read directory\n";
    write(1, error, sizeof(error) - 1);
  }
  while((entry = readdir(directory)) != NULL){
    write(1, entry->d_name, strlen(entry->d_name));
    write(1, " ", 1);
  }
  write(1, "\n", 1);

  closedir(directory);
} /*for the ls command*/

void showCurrentDir(){

} /*for the pwd command*/

void makeDir(char *dirName){

} /*for the mkdir command*/

void changeDir(char *dirName){

} /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath){

} /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath){

} /*for the mv command*/

void deleteFile(char *filename){

} /*for the rm command*/

void displayFile(char *filename){

} /*for the cat command*/
