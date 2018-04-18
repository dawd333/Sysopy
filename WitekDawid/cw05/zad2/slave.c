#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>

#define buffer_size 256

int main(int argc, char** argv){
	srand(time(NULL) * getpid());
	if(argc != 3){
		printf("Slave:Wrong number of arguments.\n");
		exit(1);
	}

	int N = strtol(argv[2], NULL, 10);
	int fifo = open(argv[1], O_WRONLY);
	if (fifo == -1){
		printf("Slave:Problem with opening fifo.\n");
		exit(1);
	}

	printf("Slave:My pid is %d\n", getpid());

	char tmp[buffer_size];
	char output[buffer_size];
	for(int i=0; i<N; i++){
		FILE* date = popen("date", "r");
		fgets(tmp, buffer_size, date);
		sprintf(output, "Slave:My pid is %d, date is %s", getpid(), tmp);
		write(fifo, output, strlen(output));
		fclose(date);
		sleep(rand()%5+2);
	}

	close(fifo);
	printf("Slave:My pid is %d, I just exited.\n", getpid());
	return 0;
}