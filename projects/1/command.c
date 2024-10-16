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

  directory = opendir(".");
  while ((entry = readdir(directory)) != NULL) {
    write(1, entry->d_name, strlen(entry->d_name));
    write(1, " ", 1);
  }
  closedir(directory);
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
  struct stat destStat;

  if (stat(destinationPath, &destStat) == 0 && S_ISDIR(destStat.st_mode)) {
    char *baseName = strrchr(sourcePath, '/');
    baseName = baseName ? baseName + 1 : sourcePath;
    
    char newDestPath[1024];
    strcpy(newDestPath, destinationPath);
    strcat(newDestPath, "/");
    strcat(newDestPath, baseName);
    destinationPath = newDestPath;
  }


  sourceFile = open(sourcePath, O_RDONLY);
  if(sourceFile < 0){
    write_message("Error! Unable to open source file for reading\n");
    return;
  }

  destinationFile = open(destinationPath, O_WRONLY | O_CREAT, 0644); // chmod access
  if(destinationFile < 0){
    write_message("Error! Unable to open destination file for writing\n");
    close(sourceFile);
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
  struct stat destStat;

  if (stat(destinationPath, &destStat) == 0 && S_ISDIR(destStat.st_mode)) {
    char *baseName = strrchr(sourcePath, '/');
    baseName = baseName ? baseName + 1 : sourcePath;
    
    char newDestPath[1024];
    strcpy(newDestPath, destinationPath);
    strcat(newDestPath, "/");
    strcat(newDestPath, baseName);
    destinationPath = newDestPath;
  }

  if(rename(sourcePath, destinationPath) != 0) {
    write_message("Error! Unable to move/rename the file or directory\n");
  }
} /*for the mv command*/

void deleteFile(char *filename){
  if(remove(filename) != 0) {
    write_message("Error! Unable to delete the file or directory\n");
  }
} /*for the rm command*/

void displayFile(char *filename){
  int fd;
  fd = open(filename, O_RDONLY);
  char buffer[1024];
  ssize_t bytesRead;

  if (fd == -1) {
    write(1, "cat: ", strlen("cat: "));
    write(1, filename, strlen(filename));
    write(1, ": ", strlen(": "));
    write(1, "No such file or directory\n", strlen("No such file or directory\n"));
  }

  while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0){
    write(1, buffer, bytesRead);
  }
  close(fd);
} /*for the cat command*/
