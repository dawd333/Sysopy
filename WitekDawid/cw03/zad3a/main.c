#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

#define max_arguments 32
#define max_size_of_task 128

void set_limits(int seconds, int megabytes){
	struct rlimit setCPU, setAS;
	setCPU.rlim_cur = (rlim_t) seconds *  3/4;
	setCPU.rlim_max = (rlim_t) seconds;
	setAS.rlim_cur = (rlim_t) megabytes * 1024 * 1024 * 3/4; // bo w bajtach
	setAS.rlim_max = (rlim_t) megabytes * 1024 * 1024;
	setrlimit(RLIMIT_CPU, &setCPU);
	setrlimit(RLIMIT_AS, &setAS);
}

int main(int argc, char **argv){
	if (argc != 4){
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
	int seconds = (int) strtol(argv[2], NULL, 10);
	int megabytes = (int) strtol(argv[3], NULL, 10);
	struct rusage previous_usage;
	getrusage(RUSAGE_CHILDREN, &previous_usage); //potrzebne bo RUSAGE_CHILDREN zwraca dla wszystkich dzieci

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
			set_limits(seconds, megabytes);
			execvp(arguments[0], arguments);
		}
		int status;
		pid = wait(&status);
		if(status != 0){
			printf("Error while running task %s.\n", arguments[0]);
			printf("Raw wait return: %d.\n", status);
			if (WIFSIGNALED(status)){
				printf("Task was terminated by signal: %d.\n", WTERMSIG(status));
				//11 - SIGSEGV Core Invalid memory reference
				//24 - SIGSTP Stop Stop typed at terminal
			}
		}
		struct rusage usage;
		getrusage(RUSAGE_CHILDREN, &usage); 
		struct timeval ru_utime;
		struct timeval ru_stime;
		timersub(&usage.ru_utime, &previous_usage.ru_utime, &ru_utime);
		timersub(&usage.ru_stime, &previous_usage.ru_stime, &ru_stime);
		previous_usage = usage;
		for (int i =0; i<current_argument; i++){
			printf("%s ", arguments[i]);
		}
		printf("\n");
		printf("User CPU time used: %d.%d seconds, system CPU time used: %d.%d seconds.\n", (int) ru_utime.tv_sec, (int) ru_utime.tv_usec, (int) ru_stime.tv_sec, (int) ru_stime.tv_usec);
	}
	fclose(file);
	return 0;
}