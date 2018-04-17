#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define max_arguments 32
#define max_size_of_task 128
#define max_commands 64


void parse_line(char** arguments){
	int number_of_commands = 1; //bo jak sa 2 || to mamy 3 komendy
	int pipes[2][2];
	int i=0,j,k=0;
	int length = 0;

	while(arguments[i] != NULL){
		i++;
		length++;
	}
	//printf("%d\n", length);

	for(i = 0; i<length; i++){
		if(strcmp(arguments[i], "|") == 0){
			number_of_commands++;
		}
	}
	//printf("%d\n", number_of_commands);

	char* commands[number_of_commands][max_arguments];
	for(i = 0; i<number_of_commands; i++){
		j = 0;
		while(arguments[k]!=NULL){
			if(strcmp(arguments[k], "|") == 0){
				k++;
				break;
			}
			commands[i][j] = arguments[k];
			//printf("%s, %d, %d, %d\n", commands[i][j], i, j, k);
			j++;
			k++;
		}
	}

	for(i = 0; i<number_of_commands; i++){
		//printf("%d\n", i);
		if(i > 0){
			close(pipes[i%2][0]);
			close(pipes[i%2][1]);
		}

		if(pipe(pipes[i%2]) == -1){
			printf("Error creating pipe.\n");
			exit(1);
		}

		pid_t cpid = fork();
		if (cpid == 0){
			//printf("Kupa %d\n", i);
			if(i != 0){
				close(pipes[(i+1)%2][1]);
				if(dup2(pipes[(i+1)%2][0], STDIN_FILENO) == -1){
					printf("Problem with dup2a.\n");
					exit(1);
				}
			}

			if(i != number_of_commands-1){
				close(pipes[(i%2)][0]);
				if(dup2(pipes[i%2][1], STDOUT_FILENO) == -1){
					printf("Problem with dup2b.\n");
					exit(1);
				}
			}
			//printf("%d, %d\n", getpid(), getppid());
			//printf("%s\n", commands[i][0]);
			execvp(commands[i][0], commands[i]);
			exit(0);
		}

		int status;
		wait(&status);
		if(status != 0){
			printf("Error while running task %s.\n", commands[i][0]);
			printf("Raw wait return: %d.\n", status);
			if (WIFSIGNALED(status)){
				printf("Task was terminated by signal: %d.\n", WTERMSIG(status));
				//11 - SIGSEGV Core Invalid memory reference
				//24 - SIGSTP Stop Stop typed at terminal
			}
		}
	}
	close(pipes[i%2][0]);
	close(pipes[i%2][1]);
	wait(NULL);
	exit(0);
}

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
			//printf("%s\n", arguments[current_argument-1]);
			if(current_argument > max_arguments){
				fprintf(stderr, "Too much arguments for task %s.\n", arguments[0]);
			}
		}

		pid_t pid = fork();
		if (pid == 0){
			parse_line(arguments);
			exit(0);
		}
		int status;
		wait(&status);
		if(status != 0){
			printf("Error while executing line.\n");
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