#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char** argv){
	if(argc != 4){
		printf("Main:Wrong number of arguments.\n");
		exit(1);
	}

	int N = strtol(argv[3], NULL, 10); //liczba procesow do stworzenia

	pid_t master = fork();
	if(master == -1){
		printf("Main:Problem with creating master.\n");
		exit(1);
	}
	if(master == 0){
		execlp("./master", "./master", argv[1], (char*) NULL);
		exit(0);
	}

	sleep(1);

	for(int i=0; i<N; i++){
		pid_t slave = fork();
		if(slave == -1){
			printf("Main:Problem with creating slave.\n");
			exit(1);
		}
		if(slave == 0){
			execlp("./slave", "./slave", argv[1], argv[2], (char*) NULL);
			exit(0);
		}
	}
	return 0;
}