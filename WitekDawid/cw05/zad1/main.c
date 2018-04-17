#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define max_arguments 32
#define max_size_of_task 128
#define max_commands 64

void parse_line(char** arguments){
	int number_of_commands = 1; //bo jak sa 2 || to mamy 3 komendy
	int pipes[2][2];
	int i=0,j,k=0,tmp;
	char* commands[max_arguments];

	while(arguments[i] != NULL){		
		if(strcmp(arguments[i], "|") == 0){
			number_of_commands++;
		}
		i++;
	}

	for(i = 0; i<number_of_commands; i++){
		j=0;
		tmp = 0;

		while (commands[tmp] != NULL){
			commands[tmp] = NULL;
			tmp++;
		}

		while(arguments[k] != NULL){
			if(strcmp(arguments[k], "|") == 0){
				k++;
				break;
			}
			commands[j] = arguments[k];
			j++;
			k++;
		}

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
			if(i != number_of_commands-1){
				close(pipes[(i%2)][0]);
				if(dup2(pipes[i%2][1], STDOUT_FILENO) == -1){
					printf("Problem with duplicating STDOUT_FILENO.\n");
					exit(1);
				}
			}

			if(i != 0){
				close(pipes[(i+1)%2][1]);
				if(dup2(pipes[(i+1)%2][0], STDIN_FILENO) == -1){
					printf("Problem with duplicating STDIN_FILENO.\n");
					exit(1);
				}
			}

			execvp(commands[0], commands);
			exit(0);
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
			if(current_argument > (max_arguments+1) * max_commands){
				printf("Too much arguments in line.\n");
				exit(1);
			}
		}

		pid_t pid = vfork();
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