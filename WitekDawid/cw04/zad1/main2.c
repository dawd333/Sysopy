#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int tstp_flag = 0;
int dead_process = 0;
pid_t pid = 0;

void tstp_signal (int sig_num){
	if(tstp_flag == 0){
		printf("\nOdebrano sygnal %d\nOczekuje na CTRL+Z - kontynuacja lub CTRL+C - zakoczenie programu\n", sig_num);
		tstp_flag = 1;
	}
	else{
		tstp_flag = 0;
	}
}

void int_signal (int sig_num){
	printf("\nOdebrano sygnal SIGINT - %d\n", sig_num);
	exit(0);
}

int main(int argc, char** argv){
	struct sigaction actions;
	actions.sa_handler = tstp_signal;
	actions.sa_flags = 0;
	sigemptyset(&actions.sa_mask);

	pid = fork();
	if (pid == 0){
		execl("./date.sh", "./date.sh", NULL);
		exit(0);
	}

	while(1){
		signal(SIGINT, int_signal);
		sigaction(SIGTSTP, &actions, NULL);

		if (tstp_flag == 0){
			if (dead_process == 1){
				dead_process = 0;

				pid = fork();
				if (pid == 0){
					execl("./date.sh", "./date.sh", NULL);
					exit(0);
				}
			}
		}
		else{
			if (dead_process == 0){
				kill(pid, SIGKILL);
				dead_process = 1;
			}
		}
	}
}