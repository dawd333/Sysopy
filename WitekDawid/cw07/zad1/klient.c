#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <wait.h>

#include "specifications.h"

int flag = 0;
int shared_memory = -1;
int semaphore = -1;
Shm *shm;

long get_time(){
  long timer;
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC,  &t);
  timer = t.tv_nsec/1000;
  return timer;
}

void change_flag(int signo){
  if(signo == SIGUSR1) flag = 1;
}

void sig_handler(int signo){
  printf("\n");
  exit(0);
}

int main(int argc, char **argv){
  if(argc != 3){
    printf("Wrong number of arguments, please enter number of clients and number of shaves.\n");
    exit(1);
  }

  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);
  signal(SIGUSR1, change_flag);

  key_t sem_key = ftok(semaphore_path, PROJECTID);
  if((semaphore = semget(sem_key, 7, IPC_CREAT | 0666)) == -1){
    printf("KLIENT: Error while creating semaphores.\n");
    exit(1);
  }

  key_t shm_key = ftok(shm_path, PROJECTID);
  if((shared_memory = shmget(shm_key, sizeof(Shm), IPC_CREAT | 0666)) == -1){
    printf("KLIENT: Error while creating shared memory.\n");
    exit(1);
  }

  if((shm = (Shm *) shmat(shared_memory, NULL, 0)) == -1){
    printf("KLIENT: Error while attaching shared memory.\n");
    exit(1);
  }

  int clients_amount = (int) strtol(argv[1], NULL, 10);
  if(clients_amount > MAXCLIENTS){
    printf("KLIENT: Too many clients passed in first argument.\n");
    exit(1);
  }
  int shaves_amount = (int) strtol(argv[2], NULL, 10);

  pid_t process_array[MAXCLIENTS];
  int l=0;

  struct sembuf wait_sem; //struktura do czekania na pozwolenie
  wait_sem.sem_op = -1;
  wait_sem.sem_flg = 0;

  struct sembuf unblock_sem; //struktura do odblokowywania
  unblock_sem.sem_op = 1;
  unblock_sem.sem_flg = 0;

  for(int i=0; i<clients_amount; i++){
    pid_t child = fork();
    if (child == 0){
      for (int j=0; j<shaves_amount; j++){
        wait_sem.sem_num = ASLEEPSEM;
        semop(semaphore, &wait_sem, 1); //czekam na zmiane flagi spania
        if(shm -> asleep == 1){ //golibroda spi czyli moge go obudzic zeby mnie strzygl
          shm -> currently_shaved = getpid();
          printf("%ld - KLIENT %d: Waking up the barber.\n", get_time(), getpid());
          unblock_sem.sem_num = WAKEUPSEM;
          semop(semaphore, &unblock_sem, 1); //budze golibrode
          wait_sem.sem_num = SITSEM;
          semop(semaphore, &wait_sem, 1); //czekam az bedzie mozna usiasc
          printf("%ld - KLIENT %d: I have just sit on the barber's chair.\n", get_time(), getpid());
          unblock_sem.sem_num = SHAVEMESEM;
          semop(semaphore, &unblock_sem, 1);  //odblokowywuje semafor strzyzenia
          wait_sem.sem_num = SHAVINGENDEDSEM;
          semop(semaphore, &wait_sem, 1); //czekam az skonczy
          printf("%ld - KLIENT %d: Leaving after %d shaving ended.\n", get_time(), getpid(), j+1);
          unblock_sem.sem_num = ILEFTSEM;
          semop(semaphore, &unblock_sem, 1);  //wychodze
        }
        else{ //golibroda nie spi wiec jest zajety, ide sprawdzac poczekalnie
          wait_sem.sem_num = SEATSARRAYSEM;
          semop(semaphore, &wait_sem, 1); //czekam az bede mogl modyfikowac tablice klientow
          if(shm -> client_counter >= shm -> seat_limit){ //nie ma miejsca w poczekalni
            printf("%ld - KLIENT %d: Leaving because there are no seats in waiting room.\n", get_time(), getpid());
            unblock_sem.sem_num = ASLEEPSEM;
            semop(semaphore, &unblock_sem, 1);
            unblock_sem.sem_num = SEATSARRAYSEM;
            semop(semaphore, &unblock_sem, 1);  //odblokowywuje sprawdzanie tablicy klientow
          }
          else{ //ustawienie sie w kolejce, dodanie liczby klientow oczekujacych
            printf("%ld - KLIENT %d: Waiting in queue for shaving.\n", get_time(), getpid());
            shm -> seats[shm -> client_counter] = getpid();
            shm -> client_counter++;
            unblock_sem.sem_num = ASLEEPSEM;
            semop(semaphore, &unblock_sem, 1);  //golibroda jest zajety sam musze odlbokowac semafory
            unblock_sem.sem_num = SEATSARRAYSEM;
            semop(semaphore, &unblock_sem, 1);  //zeby mozna bylo operowac na tablicy klientow
            while(!flag); //czekam az bede pierwszy w kolejce
            flag = 0;
            wait_sem.sem_num = SITSEM;
            semop(semaphore, &wait_sem, 1); //czekam az bedzie mozna usiasc
            printf("%ld - KLIENT %d: I have just sit on the barber's chair.\n", get_time(), getpid());
            shm -> currently_shaved = getpid();  //ustawiam kto sie strzyze
            unblock_sem.sem_num = SHAVEMESEM;
            semop(semaphore, &unblock_sem, 1);  //odblokowywuje semafor strzyzenia
            wait_sem.sem_num = SHAVINGENDEDSEM;
            semop(semaphore, &wait_sem, 1); //czekam az skonczy
            printf("%ld - KLIENT %d: Leaving after %d shaving ended.\n", get_time(), getpid(), j+1);
            unblock_sem.sem_num = ILEFTSEM;
            semop(semaphore, &unblock_sem, 1);  //wychodze
          }
        }
      }
      return 0;
    }
    else{
      process_array[l] = child;
      l++;
    }
  }
  for(int k=0; k<l; k++){
    waitpid(process_array[k], NULL, 0);
  }
  return 0;
}