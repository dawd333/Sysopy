#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "library.h"

char *global_array[1000000];


struct wrapped_array* create(int number_of_blocks, int is_static){
	if(number_of_blocks<0){
		return NULL;
	}

	struct wrapped_array* res = malloc(sizeof(struct wrapped_array));
	res -> number_of_blocks = number_of_blocks;
	res -> is_static = is_static;

	if (is_static == 1){
		res -> array = global_array;
	}
	else{
		char** array = (char**) calloc(number_of_blocks, sizeof(char*));
		res -> array = array;
	}

	return res;
}

void make_block(struct wrapped_array* array, char* block, int index){
	if(index >= array->number_of_blocks || index < 0){
		return;
	}
	else{
		array -> array[index] = calloc(strlen(block), sizeof(char));
		strcpy(array->array[index], block);
	}
}

void delete_block(struct wrapped_array *array, int index){
	if(array == NULL || array -> array[index] == NULL){
		return;
	}
	else{
		free(array -> array[index]);
		array -> array[index] = NULL;
	}
}

void delete_array(struct wrapped_array *array){
	for (int i=0; i<array->number_of_blocks; i++){
		if(array->array[i]!=NULL){
			free(array->array[i]);
		}
	}
}

int get_sum(char* block){
	int total = 0;
	int length = strlen(block);

	for (int i=0; i<length; i++){
		total+= (int) block[i];
	}
	return total;
}

char* find_nearest_sum(struct wrapped_array* array, int index){
	char* result = NULL;
	int best = INT_MAX;
	for (int i=0; i< array->number_of_blocks; i++){
		char* res = array -> array[i];
		if (res!=NULL){
			int score = get_sum(res);
			if (score < best){
				best = score;
				result = res;
			}
		}
	}
	return result;
}