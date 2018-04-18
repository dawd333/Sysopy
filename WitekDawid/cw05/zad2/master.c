#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

#define buffer_size 256

int main(int argc, char** argv){
	if(argc != 2){
		printf("Master:Wrong number of arguments.\n");
		exit(1);
	}

	if(mkfifo(argv[1], S_IRUSR | S_IWUSR) == -1){
		printf("Master:Problem with making fifo.\n");
		exit(1);
	}

	FILE *fifo = fopen(argv[1], "r");
	if (fifo == NULL){
		printf("Master:Problem with opening fifo.\n");
		exit(1);
	}

	char reader[buffer_size];

	while(fgets(reader, buffer_size, fifo) != NULL){
		printf("%s\n", reader);
	}

	printf("Master:Finished reading from fifo.\n");
	fclose(fifo);
	if(remove(argv[1]) == -1){
		printf("Master:Problem with removing fifo.\n");
		exit(1);
	}
	return 0;
}