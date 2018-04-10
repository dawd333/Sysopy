#include <stdlib.h>

int main(){
	int *memoryTest_array = malloc(sizeof(int) * 100000000);
	for (int i=0; i<100000000; i++){
		memoryTest_array[i]= 17;
	}
	free(memoryTest_array);
	return 0;
}