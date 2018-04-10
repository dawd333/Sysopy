#define _XOPEN_SOURCE 500

#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>
#include <linux/limits.h>
#include <ftw.h>

#define PATH_MAX 4096

char tmp[PATH_MAX];

double compare_date(time_t date_1, time_t date_2){
	return difftime(date_1, date_2);
}

void print_info (const char *path, const struct stat *stat_struct){
	printf(" %s\t", path);
	printf(" %ld\t", stat_struct -> st_size);
	printf((stat_struct -> st_mode & S_IRUSR) ? "r" : "-");
	printf((stat_struct -> st_mode & S_IWUSR) ? "w" : "-");
	printf((stat_struct -> st_mode & S_IXUSR) ? "x" : "-");
	printf((stat_struct -> st_mode & S_IRGRP) ? "r" : "-");
	printf((stat_struct -> st_mode & S_IWGRP) ? "w" : "-");
	printf((stat_struct -> st_mode & S_IXGRP) ? "x" : "-");
	printf((stat_struct -> st_mode & S_IROTH) ? "r" : "-");
	printf((stat_struct -> st_mode & S_IWOTH) ? "w" : "-");
	printf((stat_struct -> st_mode & S_IXOTH) ? "x" : "-");
	strftime(tmp, PATH_MAX, "%Y-%m-%d %H:%M:%S", localtime(&stat_struct -> st_mtime));
	printf(" %s\t", tmp);
	printf("\n");
}

void dir_search(char* path, char* operant, time_t date){
	if(path == NULL){
		return ;
	}

	DIR *dir = opendir(path);

	if (dir == NULL){
		printf("%s\n", "error :(");
		return;
	}

	struct dirent *read_dir = readdir(dir);
	struct stat stat_struct;

	char new_path [PATH_MAX];

	while(read_dir != NULL){
		strcpy(new_path, path);
		strcat(new_path, "/");
		strcat(new_path, read_dir -> d_name);

		lstat(new_path, &stat_struct); //lstat bo bez dowiazan

		if (strcmp(read_dir -> d_name, ".") == 0 || strcmp(read_dir -> d_name, "..") == 0){
			read_dir = readdir(dir);
			continue;
		}
		else{
			if (S_ISREG(stat_struct.st_mode)){
				if (strcmp(operant, "=") == 0 && compare_date(date, stat_struct.st_mtime) == 0){
					print_info(new_path, &stat_struct);
				}
				else if (strcmp(operant, "<") == 0 && compare_date(date, stat_struct.st_mtime) > 0){
					print_info(new_path, &stat_struct);
				}
				else if (strcmp(operant, ">") == 0 && compare_date(date, stat_struct.st_mtime) < 0){
					print_info(new_path, &stat_struct);
				}
			}

			if (S_ISDIR(stat_struct.st_mode)){
				pid_t new_process;
				new_process = fork();
				if (new_process == 0){
					dir_search(new_path, operant, date);
					exit(0);
				}
				else{
					wait(NULL);
				}
			}
			read_dir = readdir(dir);
		}
	}
	closedir(dir);
}

int main(int argc, char **argv){
	if (argc != 4){
		printf("Wrong number of arguments\n");
		return 1;
	}

	char* path = argv[1];
	char* operant = argv[2];
	char* arg_date = argv[3];

	char day[3], month[3], year[5], hour[3], min[3], sec[3];
	struct tm tm;

	memcpy(day, &arg_date[0], 2);
	memcpy(month, &arg_date[3], 2);
	memcpy(year, &arg_date[6], 4);
	memcpy(hour, &arg_date[11], 2);
	memcpy(min, &arg_date[14], 2);
	memcpy(sec, &arg_date[17], 2);
	day[2] = '\0';
	month[2] = '\0';
	year[4] = '\0';
	hour[2] = '\0';
	min[2] = '\0';
	sec[2] = '\0';

	tm.tm_year = (int) (strtol(year, NULL, 10)-1900);
	tm.tm_mday = (int) (strtol(day, NULL, 10));
	tm.tm_mon = (int) (strtol(month, NULL, 10)-1);
	tm.tm_hour = (int) (strtol(hour, NULL, 10));
	tm.tm_min = (int) (strtol(min, NULL, 10));
	tm.tm_sec = (int) (strtol(sec, NULL, 10));
	tm.tm_isdst = 0;
	time_t date = mktime(&tm);

	DIR *dir = opendir(realpath(path, NULL));
	if (dir == NULL){
		printf("Could't open the directory\n");
		return 1;
	}

	dir_search(realpath(path, NULL), operant, date);

	closedir(dir);
	return 0;
}