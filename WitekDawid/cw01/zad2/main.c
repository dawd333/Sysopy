#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

#include "library.h"


char* get_random_string(int size){
	if(size <1){
		return NULL;
	}
	char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t charset_length = strlen(charset);
	char* res = (char*) malloc((size) * sizeof(char));

	for(int i=0;i<size-1;i++){
		res[i]=charset[rand()%charset_length];
	}
	res[size-1]='\0';

	return res;
}

void fill_array(struct wrapped_array* array, int block_size){
	for (int i=0;i<array->number_of_blocks;i++){
		char* random_string = get_random_string(block_size);
		make_block(array,random_string,i);
	}
}

void make_x_blocks(struct wrapped_array* array, int amount, int start_index, int block_size){
	for(int i=0; i<amount; i++){
		char* block = get_random_string(block_size);
		make_block(array, block, start_index+i);
	}
}

void remove_x_blocks(struct wrapped_array* array, int amount, int start_index){
	for(int i=0; i<amount; i++){
		delete_block(array, start_index+i);
	}
}

void remove_and_add_all(struct wrapped_array* array, int amount, int block_size){
	remove_x_blocks(array, amount, 0);
	make_x_blocks(array, amount, 0, block_size);
}

void remove_and_add_one_by_one(struct wrapped_array* array, int amount, int block_size){
	for(int i=0; i<amount; i++){
		delete_block(array, i);
		make_block(array, get_random_string(block_size), i);
	}
}

double calculate_time(clock_t start, clock_t end){
	return (double) (end-start) / CLOCKS_PER_SEC;
}

void exec_translated_operation(char* arg, int parameter, struct wrapped_array* array, int block_size){
	if (strcmp(arg, "change_all") == 0){
		remove_and_add_all(array, parameter, block_size);
	}

	if (strcmp(arg, "change_obo") == 0){
		remove_and_add_one_by_one(array, parameter, block_size);
	}

	if (strcmp(arg, "find_nearest") == 0){
		find_nearest_sum(array, parameter);
	}

	if (strcmp(arg, "delete") == 0){
		delete_array(array);
	}
}

int main(int argc, char** argv){
	if (argc < 4){
		printf("Please try again, u have to enter type of allocation, number of blocks, block size and maximum of 2 jobs to do with their parameter");
		return 1;
	}

	int is_static;
	if (strcmp(argv[1], "static") == 0){
		is_static = 1;
	}
	else if (strcmp(argv[1], "dynamic") == 0){
		is_static = 0;
	}
	else{
		printf("Wrong type of allocation, try again ");
		return 1;
	}

	int number_of_blocks = (int) strtol(argv[2], NULL, 10);
	int block_size = (int) strtol(argv[3], NULL, 10);

	char* first_job;
	int first_parameter;
	char* second_job;
	int second_parameter;

	if (argc >= 5){
		first_job = argv[4];
	}
	if (argc >= 6){
		first_parameter = (int) strtol(argv[5], NULL, 10);
	}
	if (argc >= 7){
		second_job = argv[6];
	}
	if (argc >= 8){
		second_parameter = (int) strtol(argv[7], NULL, 10);
	}

	struct tms** tms_time = malloc(6 * sizeof(struct tms*));
	for (int i=0; i<6; i++){
		tms_time[i] = (struct tms*) malloc(sizeof(struct tms*));
	}
	clock_t real_time[6];


	printf("Real ----- User ----- System \n");

	struct wrapped_array* array;
	array = create (number_of_blocks, is_static);

	real_time[0] = times(tms_time[0]);
	fill_array(array,block_size);
	real_time[1] = times(tms_time[1]);

	printf("%s", "Creating an array\n");
	printf("%lf   ", calculate_time(real_time[0],real_time[1]));
	printf("%lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
	printf("%lf   ", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
	printf("\n");

	if (argc >= 5){
		real_time[2] = times(tms_time[2]);
		exec_translated_operation(first_job, first_parameter, array, block_size);
		real_time[3] = times(tms_time[3]);

		printf("%s %s", first_job, "\n");
		printf("%lf   ", calculate_time(real_time[2],real_time[3]));
		printf("%lf   ", calculate_time(tms_time[2]->tms_utime, tms_time[3]->tms_utime));
		printf("%lf   ", calculate_time(tms_time[2]->tms_stime, tms_time[3]->tms_stime));
		printf("\n");
	}

	if (argc >= 7){
		real_time[4] = times(tms_time[4]);
		exec_translated_operation(second_job, second_parameter, array, block_size);
		real_time[5] = times(tms_time[5]);

		printf("%s %s", second_job, "\n");
		printf("%lf   ", calculate_time(real_time[4],real_time[5]));
		printf("%lf   ", calculate_time(tms_time[4]->tms_utime, tms_time[5]->tms_utime));
		printf("%lf   ", calculate_time(tms_time[4]->tms_stime, tms_time[5]->tms_stime));
		printf("\n");
	}

	return 0;
}
