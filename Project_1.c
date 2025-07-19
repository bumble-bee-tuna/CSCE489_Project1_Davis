/*
 Minimum Viable Product for CSCE 489, Project_1, Davis

 "Simple" shell implementation in C

 This shell supports the following commands:
	- halt: exit the shell
	- dir: list files in the current directory
	- create <filename>: create a new file
	- list <filename>: display the contents of a file
	- update <filename> <number> "<text>": append text to a file multiple times
	-- can also be ran in background via & in last argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


// halt to act as exit
void halt() {
	printf("HALTED!\n");

	exit(0);
}


// dir to act as ls
void dir() {
	execl("/bin/ls", "ls", NULL);

	exit(0);
}


//create to act as touch
void create (const char *filename) {
	FILE *file = fopen(filename, "a");
	
	if (!file) {
        perror("\ncreate: could not create file\n");
        exit(1);
    }

	fclose(file);

    exit(0);
}

// list to act as cat
void list(const char *filename) {
	
    FILE *file = fopen(filename, "r");
	
    if (!file) {
        perror("list: could not open file\n");
        exit(1);
    }
	
    fclose(file);
    execl("/bin/cat", "cat", filename, NULL);
	
    exit(0);
}

// update to act as echo??
void update(const char *filename, int number, const char *text) {
    FILE *file = fopen(filename, "a");
    
    if (!file) {
        perror("update: could not open file\n");
        exit(1);
    }

	// force postive number of writes
	for (int count = 0; count < abs(number); count++) {
        fprintf(file,"%s\n", text);
        sleep(strlen(text)/5);
    }
    
    fclose(file);
    exit(0);
}

// shell setup
int main(int argc, char *argv[]) {

	char input[1024];
	
	while (1) {
		
		pid_t piddy = getpid();

		//prompt the user
		printf("This Loop PID is %d >> ", piddy);
		
		// read input from the user and ensure crtl+D exits properly, otherwise the shell is buggy
		if (fgets(input, sizeof(input), stdin) == NULL) {
			break;
		}

		// break the input into command and arguments
		input[strcspn(input, "\n")] = 0;
		char *cmd = strtok(input, " ");
		char *arg = strtok(NULL, " ");
		char *arg2 = strtok(NULL, " ");
		
		// hotfix for handling quotes in update command
		char *arg3 = strtok(NULL, "\"");

		// enable update command to run in background
		char *arg4 = strtok(NULL, " ");

		// handles unwanted input
		if (cmd == NULL) continue;

		// kills the whole shell if the user types "halt", must be before the fork
		if (strcmp(cmd, "halt") == 0) {
			halt();
		}

		// fork a new process to handle the incoming command
		pid_t pid = fork();
		//printf("Child: %d\n", pid);
		
		if (pid == 0) {
			
			if (strcmp(cmd, "dir") == 0) dir();
			
			else if (strcmp(cmd, "list") == 0 && arg != NULL) list(arg);
			
			else if (strcmp(cmd, "create") == 0 && arg != NULL) create(arg);
			
			else if (strcmp(cmd, "update") == 0 && arg != NULL && arg2 != NULL && arg3 != NULL) {

				// convert string to int since strtok returns a string
				int number = atoi(arg2);
				update(arg, number, arg3);
			}
			
			else {

				// if the command is not recognized, print an error message and exit
				printf("Unknown command or command with no arguments!\n");

				// does it matter to exit(0) or exit(1) here??
				exit(1);
			}
			
		} else {
			// Parent process - wait for child to finish (unless background)
			if (arg4 == NULL || strcmp(arg4, "&") != 0) {
				waitpid(pid, NULL, 0);
			}
		}
	}
}
