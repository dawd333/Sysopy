#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#include "specifications.h"

int shared_memory = -1;
int semaphore = -1;
Shm* shm;

long get_time(){
  long timer;
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC,  &t);
  timer = t.tv_nsec/1000;
  return timer;
}

void sig_handler(int signo){
  printf("\n");
  exit(0);
}

void remove_shm_and_sem(){
  printf("GOLIBRODA: Clearing before exit.\n");
  if(shmctl(shared_memory, IPC_RMID, NULL) == -1) printf("GOLIBRODA: Error while deleting shared memory.\n");
  if(semctl(semaphore, 0, IPC_RMID, NULL) == -1) printf("GOLIBRODA: Error while deleting semaphores.\n");
}

int main(int argc, char **argv){
  if(argc != 2){
    printf("GOLIBRODA: Wrong number of arguments, please enter only number of seats.\n");
    exit(1);
  }

  if(atexit(remove_shm_and_sem) != 0){
    printf("GOLIBRODA: Error while registering atexit function\n");
    exit(1);
  }

  int seat_limit = (int) strtol(argv[1], NULL, 10);
  if(seat_limit > MAXCLIENTS){
    printf("GOLIBRODA: Seat limit is too big.\n");
    exit(1);
  }
  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);

  key_t sem_key = ftok(semaphore_path, PROJECTID);
  if((semaphore = semget(sem_key, 7, IPC_CREAT | 0666)) == -1){
    printf("GOLIBRODA: Error while creating semaphores.\n");
    exit(1);
  }

  key_t shm_key = ftok(shm_path, PROJECTID);
  if((shared_memory = shmget(shm_key, sizeof(Shm), IPC_CREAT | 0666)) == -1){
    printf("GOLIBRODA: Error while creating shared memory.\n");
    exit(1);
  }

  if((shm = (Shm *) shmat(shared_memory, NULL, 0)) == -1){
    printf("GOLIBRODA: Error while attaching shared memory.\n");
    exit(1);
  }

  shm -> seat_limit = seat_limit;
  shm -> client_counter = 0;
  for(int i=0; i<seat_limit; i++){
    shm -> seats[i] = 0;
  }

  for (int i=0; i<2; i++){  //poczatkowe odblokowanie semaforow golibrody
    semctl(semaphore, i, SETVAL, 1);
  }
  for (int i=2; i<7; i++){  //poczatkowe zablokowanie semaforow strzyzenia
    semctl(semaphore, i, SETVAL, 0);
  }

  struct sembuf wait_sem; //struktura do czekania na pozwolenie
  wait_sem.sem_op = -1;
  wait_sem.sem_flg = 0;

  struct sembuf unblock_sem; //struktura do odblokowywania
  unblock_sem.sem_op = 1;
  unblock_sem.sem_flg = 0;

  while(1){
    wait_sem.sem_num = ASLEEPSEM;
    semop(semaphore, &wait_sem, 1); //zablokowanie sprawdzenia czy spi
    wait_sem.sem_num = SEATSARRAYSEM;
    semop(semaphore, &wait_sem, 1); //zablokowanie sprawdzenia poczekalni z klientami
    printf("%d clients left.\n", shm -> client_counter);

    if(shm -> client_counter == 0){ //brak klientow
      unblock_sem.sem_num = SEATSARRAYSEM;
      semop(semaphore, &unblock_sem, 1);  //odblokowanie modyfikacji tablicy z klientami
      printf("%ld - GOLIBRODA: Falling asleep.\n", get_time());
      shm -> asleep = 1;
      unblock_sem.sem_num = ASLEEPSEM;
      semop(semaphore, &unblock_sem, 1);  //odblokowanie sprawdzenia czy spi
      wait_sem.sem_num = WAKEUPSEM;
      semop(semaphore, &wait_sem, 1); //czekanie az go ktos obudzi
      shm -> asleep = 0;
      unblock_sem.sem_num = ASLEEPSEM;
      semop(semaphore, &unblock_sem, 1); // klient zablokowal flage po dowiedzeniu sie ze spie, trzeba odblokowac
      printf("%ld - GOLIBRODA: Waking up.\n", get_time());
      unblock_sem.sem_num = SITSEM;
      semop(semaphore, &unblock_sem, 1); //pozwalam usiasc
      wait_sem.sem_num = SHAVEMESEM;
      semop(semaphore, &wait_sem, 1); //czekam az usiadzie
      printf("%ld - GOLIBRODA: Shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      printf("%ld - GOLIBRODA: Ended shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      unblock_sem.sem_num = SHAVINGENDEDSEM;
      semop(semaphore, &unblock_sem, 1);  //pozwalam odejsc
      wait_sem.sem_num = ILEFTSEM;
      semop(semaphore, &wait_sem, 1); //czekam az klient wyjdzie
    } 
    else {  //jest jakis klient w kolejce
      printf("%ld - GOLIBRODA: Inviting client with PID: %d.\n", get_time(), shm->seats[0]);
      kill(shm -> seats[0], SIGUSR1);
      unblock_sem.sem_num = ASLEEPSEM;
      semop(semaphore, &unblock_sem, 1);  //odblokowanie sprawdzenia czy spi, i tak jest zajety
      unblock_sem.sem_num = SITSEM;
      semop(semaphore, &unblock_sem, 1);  //pozwalam usiasc
      for (int i=0; i<shm -> client_counter; i++){  //przesuwam kolejke w poczekalni
        shm -> seats[i] = shm -> seats[i+1];
      }
      shm -> client_counter--;  //zmniejszam ilosc klientow oczekujacych
      unblock_sem.sem_num = SEATSARRAYSEM;
      semop(semaphore, &unblock_sem, 1);  //mozna sprawdzac tablice klientow
      wait_sem.sem_num = SHAVEMESEM;
      semop(semaphore, &wait_sem, 1); //czekam az usiadzie
      printf("%ld - GOLIBRODA: Shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      printf("%ld - GOLIBRODA: Ended shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      unblock_sem.sem_num = SHAVINGENDEDSEM;
      semop(semaphore, &unblock_sem, 1);  //pozwalam odejsc
      wait_sem.sem_num = ILEFTSEM;
      semop(semaphore, &wait_sem, 1); //czekam az klient wyjdzie
    }
  }
  return 0;
}