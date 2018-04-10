#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define max_arguments 32
#define max_size_of_task 128

int main(int argc, char **argv){
	if (argc != 2){
		printf("%s", "Wrong number of arguments.\n");
		return 1;
	}

	FILE *file = fopen(argv[1], "r");
	if (file == NULL){
		printf("%s", "Couldn't open a file.\n");
		return 1;
	}

	char task[max_size_of_task];
	char *arguments[max_arguments];
	int current_argument = 0;

	while(fgets(task,max_size_of_task,file) != NULL){
		current_argument = 0;

		while((arguments[current_argument] = strtok(current_argument == 0 ? task : NULL, " \n\t")) != NULL){
			current_argument++;
			if(current_argument > max_arguments){
				fprintf(stderr, "Too much arguments for task %s.\n", arguments[0]);
			}
		}
		pid_t pid = fork();
		if (pid == 0){
			execvp(arguments[0], arguments);
		}
		int status;
		wait(&status);
		if(status != 0){
			printf("Error while running task %s.\n", arguments[0]);
			printf("Raw wait return: %d.\n", status);
			if (WIFSIGNALED(status)){
				printf("Task was terminated by signal: %d.\n", WTERMSIG(status));
				//11 - SIGSEGV Core Invalid memory reference
				//24 - SIGSTP Stop Stop typed at terminal
			}
		}
	}
	fclose(file);
	return 0;
}