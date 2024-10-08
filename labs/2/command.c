#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
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
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ // Skip the . and .. directories
			continue;
		}
		if(entry->d_type == DT_REG){
			write_message("File: ");
		}
		write(1, entry->d_name, strlen(entry->d_name));
		write(1, "\n", 1);
	}
		/* You can debug by printing out the filenames here */

		/* Option: use an if statement to skip any names that are not readable files (e.g. ".", "..", "main.c", "lab2.exe", "output.txt" */
			
			/* Open the file */
			
			/* Read in each line using getline() */
				/* Write the line to stdout */
			
			/* write 80 "-" characters to stdout */
			
			/* close the read file and free/null assign your line buffer */
	
	/*close the directory you were reading from using closedir() */
}
