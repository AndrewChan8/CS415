#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "command.h"

void write_message(char *message){// Helper function so I don't have to make a char[] everytime I want to write a message
  write(1, message, strlen(message));
}

int compare(const void *a, const void *b) {
  return strcasecmp(*(const char **)a, *(const char **)b);
}

void listDir(){
  DIR *directory;
  struct dirent *entry;
  char *filenames[128]; // Store the file names to then be sorted
  int count = 0;

  directory = opendir(".");
  if(directory == NULL){
    char error[] = "Unable to read directory\n";
    write(1, error, sizeof(error) - 1);
  }
  while((entry = readdir(directory)) != NULL){
    filenames[count] = strdup(entry->d_name);
    count++;
  }
  closedir(directory);

  qsort(filenames, count, sizeof(char *), compare);

  for(int i = 0; i < count; i++){
    write(1, filenames[i], strlen(filenames[i]));
    write(1, " ", 1);
    free(filenames[i]);
  }
  write(1, "\n", 1);
} /*for the ls command*/

void showCurrentDir(){
  char cwd[2048];
  if(getcwd(cwd, sizeof(cwd)) != NULL){
    write(1, cwd, strlen(cwd));
    write(1, "\n", 1);
  }else{
    write_message("Error! Current directory could not be retrieved\n");
  }
} /*for the pwd command*/

void makeDir(char *dirName){
  if(mkdir(dirName, 0755) != 0){ // 755 is chmod 755
    switch(errno){
      case EEXIST:
        write_message("Directory already exists!\n");
        break;
      default:
        write_message("Error creating directory!\n");
        break;
    }

  }
} /*for the mkdir command*/

void changeDir(char *dirName){
  if(chdir(dirName) != 0){
    switch(errno){
      case ENOENT:
        write_message("Error! Directory does not exist\n");
        break;
      case ENOTDIR:
        write_message("Error! ");
        write_message(dirName);
        write_message(" is not a directory\n");
        break;
      default:
        write_message("Error! Unable to change to directory: ");
        write_message(dirName);
        write(1, "\n", 1);
        break;
    }
  }
} /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath){
  int sourceFile, destinationFile;
  char buffer[1024];
  ssize_t bytesRead;

  sourceFile = open(sourcePath, O_RDONLY);
  if(sourceFile < 0){
    write_message("Error! Unable to open source file for reading\n");
    return;
  }

  destinationFile = open(destinationPath, O_WRONLY | O_CREAT, 0644); // chmod access
  if(destinationFile < 0){
    write_message("Error! Unable to open destination file for writing\n");
    return;
  }

  while((bytesRead = read(sourceFile, buffer, sizeof(buffer))) > 0){
    if(write(destinationFile, buffer, bytesRead) != bytesRead){
      write_message("Error! Unable to write data to destination file\n");
      break;
    }
  }

  if(bytesRead < 0){
    write_message("Error! Unable to read data from source file\n");
  }

  close(sourceFile);
  close(destinationFile);
} /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath){

} /*for the mv command*/

void deleteFile(char *filename){

} /*for the rm command*/

void displayFile(char *filename){

} /*for the cat command*/
