#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define lib_mode 0
#define sys_mode 1

void get_random_string(char* res, int size){
	char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t charset_length = strlen(charset);

	for(int i=0;i<size;i++){
		res[i]=charset[rand()%charset_length];
	}
}

int generate (const char* path, int amount, int length){
	FILE *f = fopen(path, "w+");
	if (f == NULL){
		perror("Error with opening file described in path.");
		return 1;
	}

	char *res = malloc(length * sizeof(char) + 1);

	for (int i=0; i<amount; i++){
		get_random_string(res, length);
		res[length] = 10; //znak konca linii

		if (fwrite(res, sizeof(char), (size_t) length+1, f) != length+1){
			return 1;
		}
	}

	free(res);
	fclose(f);
	return 0;
}

void generate_tester (const char* path, int amount, int length){
	if (generate(path, amount, length) ==1){
		printf("%s\n", "There was some error with generating file.");
	}
}

int lib_sort (const char* path, int amount, int length){
	FILE *f = fopen(path, "r+");
	if (f == NULL){
		perror("Error with opening file described in path.");
		return 1;
	}

	char *res1 = malloc((length+1)* sizeof(char));
	char *res2 = malloc((length+1) * sizeof(char));

	long int offset = (long int) ((length+1) * sizeof(char));

	for (int i=0; i<amount; i++){
		fseek(f, i * offset, 0);

		if(fread(res1, sizeof(char), (size_t) length+1, f) != length+1){
			return 1;
		}

		for (int j=0; j<i; j++){
			fseek(f, j * offset, 0);
			if(fread(res2, sizeof(char), (size_t) length+1, f) != length+1){
				return 1;
			}

			if (res2[0] > res1[0]){
				fseek(f, i * offset, 0);
				if(fwrite(res2, sizeof(char), (size_t) length+1, f) != length+1){
					return 1;
				}

				fseek(f, j * offset, 0);
				if(fwrite(res1, sizeof(char), (size_t) length+1, f) != length+1){
					return 1;
				}

				char *tmp = res1;
				res1 = res2;
				res2 = tmp;
			}
		}
	}

	free(res1);
	free(res2);
	fclose(f);
	return 0;
}

int sys_sort (const char* path, int amount, int length){
	int f = open(path, O_RDWR);

	char *res1 = malloc((length+1) * sizeof(char));
	char *res2 = malloc((length+1) * sizeof(char));

	long int offset = (long int) ((length+1) * sizeof(char));

	for (int i=0; i<amount; i++){
		lseek(f, i * offset, SEEK_SET);

		if(read(f, res1, (size_t)(length+1) * sizeof(char)) != length+1){
			return 1;
		}

		for (int j=0; j<i; j++){
			lseek(f, j * offset, SEEK_SET);
			if(read(f, res2, (size_t)(length+1) * sizeof(char)) != length+1){
				return 1;
			}

			if (res2[0] > res1[0]){
				lseek(f, i * offset, SEEK_SET);;
				if(write(f, res2, (size_t)(length+1) * sizeof(char)) != length+1){
					return 1;
				}

				lseek(f, j * offset, SEEK_SET);
				if(write(f, res1, (size_t)(length+1) * sizeof(char)) != length+1){
					return 1;
				}

				char *tmp = res1;
				res1 = res2;
				res2 = tmp;
			}
		}
	}

	free(res1);
	free(res2);
	close(f);
	return 0;
}

void sort_tester (const char* path, int amount, int length, int mode){
	if (mode == lib_mode){
		if(lib_sort(path, amount, length) == 1){
			printf("%s\n", "There was some error with sorting.");
		}
	}
	else{
		if(sys_sort(path, amount, length) == 1){
			printf("%s\n", "There was some error with sorting.");
		}
	}
}

int lib_copy (const char* path, const char* destination, int amount, int length){
	FILE *source = fopen(path, "r");
	FILE *copy = fopen(destination, "w+");
	char *tmp = malloc((length+1) * sizeof(char));

	for (int i=0; i<amount; i++){
		if(fread(tmp, sizeof(char), (size_t) length+1, source) != length+1){
			return 1;
		}

		if(fwrite(tmp, sizeof(char), (size_t) length+1, copy) != length+1){
			return 1;
		}
	}

	free(tmp);
	fclose(source);
	fclose(copy);
	return 0;
}

int sys_copy (const char* path, const char* destination, int amount, int length){
	int source = open(path, O_RDONLY);
	int copy = open(destination, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	char *tmp = malloc((length+1) * sizeof(char));

	for (int i=0; i<amount; i++){
		if(read(source, tmp, (size_t)(length+1) * sizeof(char)) != length+1){
			return 1;
		}

		if(write(copy, tmp, (size_t)(length+1) * sizeof(char)) != length+1){
			return 1;
		}
	}

	free(tmp);
	close(source);
	close(copy);
	return 0;
}

void copy_tester (const char* path, const char* destination, int amount, int length, int mode){
	if (mode == lib_mode){
		if(lib_copy(path, destination, amount, length) == 1){
			printf("%s\n", "There was some error with copying.");
		}
	}
	else{
		if(sys_copy(path, destination, amount, length) == 1){
			printf("%s\n", "There was some error with copying.");
		}
	}
}

int main(int argc, char** argv){
	if(argc < 5 || argc > 7){
		printf("%s\n", "Wrong amount of arguments." );
		return 1;
	}

	struct tms tms1, tms2;

	if(strcmp(argv[1], "generate") == 0){
		int amount = (int) strtol(argv[3], NULL, 10);
		int length = (int) strtol(argv[4], NULL, 10);

		generate_tester(argv[2], amount, length);
	}

	else if(strcmp(argv[1], "sort") == 0){
		if(argc < 6){
			printf("%s\n", "Too few arguments were inserted." );
			return 1;
		}

		int amount = (int) strtol(argv[3], NULL, 10);
		int length = (int) strtol(argv[4], NULL, 10);

		if(strcmp(argv[5], "sys") == 0){
			clock_t clktms1 = times(&tms1);
			sort_tester(argv[2], amount, length, sys_mode);
			clock_t clktms2 = times(&tms2);
		}
		else if(strcmp(argv[5], "lib") == 0){
			clock_t clktms1 = times(&tms1);
			sort_tester(argv[2], amount, length, lib_mode);
			clock_t clktms2 = times(&tms2);

		}
		else{
			printf("%s\n", "No such mode.");
		}
		printf("Funkcja: %s, typ: %s, ilosc rekordow: %s, dlugosc rekordu: %s\n", argv[1], argv[5], argv[3], argv[4]);
		printf("Czas uzytkownika: %.10f, czas systemowy: %.10f\n", (double)(tms2.tms_utime - tms1.tms_utime)/sysconf(_SC_CLK_TCK), (double)(tms2.tms_stime - tms1.tms_stime)/sysconf(_SC_CLK_TCK));

	}

	else if(strcmp(argv[1], "copy") == 0){
		if(argc < 7){
			printf("%s\n", "Too few arguments were inserted." );
			return 1;
		}

		int amount = (int) strtol(argv[4], NULL, 10);
		int length = (int) strtol(argv[5], NULL, 10);

		if(strcmp(argv[6], "sys") == 0){
			clock_t clktms1 = times(&tms1);
			copy_tester(argv[2], argv[3], amount, length, sys_mode);
			clock_t clktms2 = times(&tms2);
		}
		else if(strcmp(argv[6], "lib") == 0){
			clock_t clktms1 = times(&tms1);
			copy_tester(argv[2], argv[3], amount, length, lib_mode);
			clock_t clktms2 = times(&tms2);
		}
		else{
			printf("%s\n", "No such mode.");
		}

		printf("Funkcja: %s, typ: %s, ilosc rekordow: %s, dlugosc rekordu: %s\n", argv[1], argv[6], argv[4], argv[5]);
		printf("Czas uzytkownika: %.10f, czas systemowy: %.10f\n", (double)(tms2.tms_utime - tms1.tms_utime)/sysconf(_SC_CLK_TCK), (double)(tms2.tms_stime - tms1.tms_stime)/sysconf(_SC_CLK_TCK));
	}

	else{
		printf("%s\n", "You entered something wrong." );
	}

	return 0;
}