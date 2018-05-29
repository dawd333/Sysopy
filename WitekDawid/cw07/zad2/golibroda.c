#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "specifications.h"

int shared_memory = -1;
sem_t *seatsarraysem;
sem_t *asleepsem;
sem_t *wakeupsem;
sem_t *sitsem;
sem_t *shavemesem;
sem_t *shavingendedsem;
sem_t *ileftsem;
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
  if(close(shared_memory) == -1) printf("GOLIBRODA: Error while closing shared memory.\n");
  if(shm_unlink(shm_path) == -1) printf("GOLIBRODA: Error while unlinking shared memory.\n");
  if(sem_close(seatsarraysem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(seatsarray) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
  if(sem_close(asleepsem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(asleep) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
  if(sem_close(wakeupsem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(wakeup) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
  if(sem_close(sitsem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(sit) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
  if(sem_close(shavemesem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(shaveme) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
  if(sem_close(shavingendedsem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(shavingended) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
  if(sem_close(ileftsem) == -1) printf("GOLIBRODA: Error while closing semaphore.\n");
  if(sem_unlink(ileft) == -1) printf("GOLIBRODA: Error while unlinking semaphore.\n");
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

  if((seatsarraysem = sem_open(seatsarray, O_CREAT, 0666, 1)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }
  if((asleepsem = sem_open(asleep, O_CREAT, 0666, 1)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }
  if((wakeupsem = sem_open(wakeup, O_CREAT, 0666, 0)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }
  if((sitsem = sem_open(sit, O_CREAT, 0666, 0)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }
  if((shavemesem = sem_open(shaveme, O_CREAT, 0666, 0)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }
  if((shavingendedsem = sem_open(shavingended, O_CREAT, 0666, 0)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }
  if((ileftsem = sem_open(ileft, O_CREAT, 0666, 0)) == SEM_FAILED){
    printf("GOLIBRODA: Error while creating semaphore.\n");
    exit(1);
  }

  if((shared_memory = shm_open(shm_path, O_CREAT | O_RDWR | O_TRUNC, 0666)) == -1){
    printf("GOLIBRODA: Error while creating shared memory.\n");
    exit(1);
  }
  ftruncate(shared_memory, sizeof(Shm));

  if((shm = (Shm *) mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory, 0)) == MAP_FAILED){
    printf("GOLIBRODA: Error while attaching shared memory.\n");
    exit(1);
  }

  shm -> seat_limit = seat_limit;
  shm -> client_counter = 0;
  for(int i=0; i<seat_limit; i++){
    shm -> seats[i] = 0;
  }

  while(1){
    sem_wait(asleepsem); //zablokowanie sprawdzenia czy spi
    sem_wait(seatsarraysem); //zablokowanie sprawdzenia poczekalni z klientami
    printf("%d clients left.\n", shm -> client_counter);

    if(shm -> client_counter == 0){ //brak klientow
      sem_post(seatsarraysem);  //odblokowanie modyfikacji tablicy z klientami
      printf("%ld - GOLIBRODA: Falling asleep.\n", get_time());
      shm -> asleep = 1;
      sem_post(asleepsem);  //odblokowanie sprawdzenia czy spi
      sem_wait(wakeupsem); //czekanie az go ktos obudzi
      shm -> asleep = 0;
      sem_post(asleepsem); // klient zablokowal flage po dowiedzeniu sie ze spie, trzeba odblokowac
      printf("%ld - GOLIBRODA: Waking up.\n", get_time());
      sem_post(sitsem); //pozwalam usiasc
      sem_wait(shavemesem); //czekam az usiadzie
      printf("%ld - GOLIBRODA: Shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      printf("%ld - GOLIBRODA: Ended shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      sem_post(shavingendedsem);  //pozwalam odejsc
      sem_wait(ileftsem); //czekam az klient wyjdzie
    } 
    else {  //jest jakis klient w kolejce
      printf("%ld - GOLIBRODA: Inviting client with PID: %d.\n", get_time(), shm->seats[0]);
      kill(shm -> seats[0], SIGUSR1);
      sem_post(asleepsem); //odblokowanie sprawdzenia czy spi, i tak jest zajety
      sem_post(sitsem);  //pozwalam usiasc
      for (int i=0; i<shm -> client_counter; i++){  //przesuwam kolejke w poczekalni
        shm -> seats[i] = shm -> seats[i+1];
      }
      shm -> client_counter--;  //zmniejszam ilosc klientow oczekujacych
      sem_post(seatsarraysem);  //mozna sprawdzac tablice klientow
      sem_wait(shavemesem); //czekam az usiadzie
      printf("%ld - GOLIBRODA: Shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      printf("%ld - GOLIBRODA: Ended shaving client with PID: %d.\n", get_time(), shm -> currently_shaved);
      sem_post(shavingendedsem);  //pozwalam odejsc
      sem_wait(ileftsem); //czekam az klient wyjdzie
    }
  }
  return 0;
}