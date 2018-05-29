#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "specifications.h"

int flag = 0;
int shared_memory = -1;
sem_t *seatsarraysem;
sem_t *asleepsem;
sem_t *wakeupsem;
sem_t *sitsem;
sem_t *shavemesem;
sem_t *shavingendedsem;
sem_t *ileftsem;
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

void remove_shm_and_sem(){
  printf("KLIENT: Clearing before exit.\n");
  if(close(shared_memory) == -1) printf("KLIENT: Error while closing shared memory.\n");
  if(sem_close(seatsarraysem) == -1) printf("KLIENT: Error while closing semaphore.\n");
  if(sem_close(asleepsem) == -1) printf("KLIENT: Error while closing semaphore.\n");
  if(sem_close(wakeupsem) == -1) printf("KLIENT: Error while closing semaphore.\n");
  if(sem_close(sitsem) == -1) printf("KLIENT: Error while closing semaphore.\n");
  if(sem_close(shavemesem) == -1) printf("KLIENT: Error while closing semaphore.\n");
  if(sem_close(shavingendedsem) == -1) printf("KLIENT: Error while closing semaphore.\n");
  if(sem_close(ileftsem) == -1) printf("KLIENT: Error while closing semaphore.\n");
}

int main(int argc, char **argv){
  if(argc != 3){
    printf("Wrong number of arguments, please enter number of clients and number of shaves.\n");
    exit(1);
  }

  if(atexit(remove_shm_and_sem) != 0){
    printf("KLIENT: Error while registering atexit function\n");
    exit(1);
  }

  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);
  signal(SIGUSR1, change_flag);

  if((seatsarraysem = sem_open(seatsarray, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }
  if((asleepsem = sem_open(asleep, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }
  if((wakeupsem = sem_open(wakeup, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }
  if((sitsem = sem_open(sit, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }
  if((shavemesem = sem_open(shaveme, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }
  if((shavingendedsem = sem_open(shavingended, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }
  if((ileftsem = sem_open(ileft, 0666)) == SEM_FAILED){
    printf("KLIENT: Error while opening semaphore.\n");
    exit(1);
  }

  if((shared_memory = shm_open(shm_path, O_RDWR, 0666)) == -1){
    printf("KLIENT: Error while opening shared memory.\n");
    exit(1);
  }

  if((shm = (Shm *) mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory, 0)) == MAP_FAILED){
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

  for(int i=0; i<clients_amount; i++){
    pid_t child = fork();
    if (child == 0){
      for (int j=0; j<shaves_amount; j++){
        sem_wait(asleepsem); //czekam na zmiane flagi spania
        if(shm -> asleep == 1){ //golibroda spi czyli moge go obudzic zeby mnie strzygl
          shm -> currently_shaved = getpid();
          printf("%ld - KLIENT %d: Waking up the barber.\n", get_time(), getpid());
          sem_post(wakeupsem); //budze golibrode
          sem_wait(sitsem); //czekam az bedzie mozna usiasc
          printf("%ld - KLIENT %d: I have just sit on the barber's chair.\n", get_time(), getpid());
          sem_post(shavemesem);  //odblokowywuje semafor strzyzenia
          sem_wait(shavingendedsem); //czekam az skonczy
          printf("%ld - KLIENT %d: Leaving after %d shaving ended.\n", get_time(), getpid(), j+1);
          sem_post(ileftsem);  //wychodze
        }
        else{ //golibroda nie spi wiec jest zajety, ide sprawdzac poczekalnie
          sem_wait(seatsarraysem); //czekam az bede mogl modyfikowac tablice klientow
          if(shm -> client_counter >= shm -> seat_limit){ //nie ma miejsca w poczekalni
            printf("%ld - KLIENT %d: Leaving because there are no seats in waiting room.\n", get_time(), getpid());
            sem_post(asleepsem);
            sem_post(seatsarraysem);  //odblokowywuje sprawdzanie tablicy klientow
          }
          else{ //ustawienie sie w kolejce, dodanie liczby klientow oczekujacych
            printf("%ld - KLIENT %d: Waiting in queue for shaving.\n", get_time(), getpid());
            shm -> seats[shm -> client_counter] = getpid();
            shm -> client_counter++;
            sem_post(asleepsem);  //golibroda jest zajety sam musze odlbokowac semafory
            sem_post(seatsarraysem);  //zeby mozna bylo operowac na tablicy klientow
            while(!flag); //czekam az bede pierwszy w kolejce
            flag = 0;
            sem_wait(sitsem); //czekam az bedzie mozna usiasc
            printf("%ld - KLIENT %d: I have just sit on the barber's chair.\n", get_time(), getpid());
            shm -> currently_shaved = getpid();  //ustawiam kto sie strzyze
            sem_post(shavemesem);  //odblokowywuje semafor strzyzenia
            sem_wait(shavingendedsem); //czekam az skonczy
            printf("%ld - KLIENT %d: Leaving after %d shaving ended.\n", get_time(), getpid(), j+1);
            sem_post(ileftsem);  //wychodze
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