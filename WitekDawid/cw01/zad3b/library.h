#ifndef LIBRARY_H
#define LIBRARY_H

struct wrapped_array{
	int number_of_blocks;
	char** array;
	int is_static;
};

struct wrapped_array* create(int number_of_blocks, int is_static);
void make_block(struct wrapped_array* array, char* block, int index);
void delete_block(struct wrapped_array* array, int index);
void delete_array(struct wrapped_array* array);
int get_sum(char* block);
char* find_nearest_sum(struct wrapped_array* array, int index);

#endif