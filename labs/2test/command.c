#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include "command.h"

void write_message(char *message){
	write(1, message, strlen(message));
}

void lfcat()
{
/* High level functionality you need to implement: */
	/* Get the current directory with getcwd() */
	size_t size = 1024;
	char *cwd = malloc(size);
	
	if(getcwd(cwd, size) == NULL){
		write_message("getcwd() error");
		free(cwd);
		return;
	}

	/* Open the dir using opendir() */
	DIR *directory;
	struct dirent *entry;

	directory = opendir(cwd);
	if(directory == NULL){
		write_message("opendir() error");
		free(cwd);
		return;
	}
	/* use a while loop to read the dir with readdir()*/
	while((entry = readdir(directory)) != NULL){
		/* Option: use an if statement to skip any names that are not readable files (e.g. ".", "..", "main.c", "lab2.exe", "output.txt" */
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ // Skip the . and .. directories
			continue;
		}
		write(1, entry->d_name, strlen(entry->d_name));
		write(1, "\n", 1);

		/* Open the file */
		int fd;
		fd = open(entry->d_name, O_RDONLY);

		/* Read in each line using getline() */
		/* Write the line to stdout */
		size_t bytes = 1024;
		char buffer[bytes];
		while((bytes = read(fd, buffer, bytes)) != 0){
			write(1, buffer, bytes);
		}
		/* close the read file and free/null assign your line buffer */
		close(fd);

		/* write 80 "-" characters to stdout */
		for(int i = 0; i < 80; i++){
			write(1, "-", 1);
		}
		write(1, "\n", 1);

	}
			
	/*close the directory you were reading from using closedir() */
	closedir(directory);	
			
	write(1, "\n", 1);
}
