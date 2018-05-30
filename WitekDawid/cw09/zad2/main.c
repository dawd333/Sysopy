#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>

int P, K, N, L, nk;
char *search_mode, *print_mode;
FILE *source_file;
char** buffer = NULL;
int write_index=0, read_index=0, buffer_slots_taken=0;
sem_t buffer_sem;
sem_t free_slots_sem;
sem_t taken_slots_sem;

void exit_handler(){
  printf("Clearing before exit.\n");
  if (buffer) free(buffer);
  if (source_file) fclose(source_file);
  sem_destroy(&buffer_sem);
  sem_destroy(&free_slots_sem);
  sem_destroy(&taken_slots_sem);
}

void int_handler(int signo){
  printf("\n");
  exit(0);
}

void alrm_handler(int signo){
  printf("Exiting after %d seconds passed.\n", nk);
  exit(0);
}

void load_config(char *file_path){
  FILE *file;
  if((file = fopen(file_path, "r")) == NULL){
    printf("Couldn't open file with configuration.\n");
    exit(1);
  }
  char buff[1024];
  fread(buff, 1024, 1, file);
  char* tmp;

  if((tmp = strtok(buff, " ")) == NULL){
    printf("Error reading a value of P.\n");
    exit(1);
  }
  if((P = (int) strtol(tmp, NULL, 10)) == 0){
    printf("Error converting the value of P.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a value of K.\n");
    exit(1);
  }
  if((K = (int) strtol(tmp, NULL, 10)) == 0){
    printf("Error converting the value of K.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a value of N.\n");
    exit(1);
  }
  if((N = (int) strtol(tmp, NULL, 10)) == 0){
    printf("Error converting the value of N.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a path to text file.\n");
    exit(1);
  }
  if((source_file = fopen(tmp, "r")) == NULL){
    printf("Couln't open text file.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a value of L.\n");
    exit(1);
  }
  if((L = (int) strtol(tmp, NULL, 10)) == 0){
    printf("Error converting the value of L.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a value of search_mode.\n");
    exit(1);
  }
  search_mode = tmp;
  if((strcmp(search_mode, "LESS") == 0 || strcmp(search_mode, "EQUAL") == 0 || strcmp(search_mode, "GREATER") == 0) == 0){
    printf("Wrong search mode.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a value of print_mode.\n");
    exit(1);
  }
  print_mode = tmp;
  if((strcmp(print_mode, "ALL") == 0 || strcmp(print_mode, "SIMPLE") == 0) == 0){
    printf("Wrong print mode.\n");
    exit(1);
  }

  if((tmp = strtok(NULL, " ")) == NULL){
    printf("Error reading a value of nk.\n");
    exit(1);
  }
  nk = (int) strtol(tmp, NULL, 10);

  fclose(file);
}

void* prod_routine(void *arg){
  char *buff = NULL;
  size_t n = 0;
  while(1){
    sem_wait(&free_slots_sem);
    sem_wait(&buffer_sem);   
    if(getline(&buff, &n, source_file) == -1){
      sem_post(&buffer_sem);
      break;
    }
    if(strcmp(print_mode, "ALL") == 0){
      printf("Producer: I have just put line into %d index of buffer. There are %d/%d slots taken.\n", write_index, buffer_slots_taken+1, N);
    } 
    buffer[write_index] = buff;
    write_index = (write_index+1)%N;
    buffer_slots_taken++;
    sem_post(&taken_slots_sem);
    sem_post(&buffer_sem);
    n=0;
    buff = NULL;
  }
  if(buff) free(buff);
  return NULL;
}

void cons_print(char *buff, int read_index){
  if(buff[strlen(buff)-1] == '\n'){
    buff[strlen(buff)-1] = '\0';
  }

  if(strlen(buff) == 0){
    if(strcmp(print_mode, "ALL") == 0){
      printf("Consumer: The line I have just read, didn't pass the requirements.\n");
    }  
    return;
  }

  else if(strcmp(search_mode, "LESS") == 0){
    int flag = (strlen(buff) < L);
    if(flag){
      printf("Consumer: Index: %d, Length: %ld, Line: %s\n", read_index, strlen(buff), buff);
    }
    else{
      if(strcmp(print_mode, "ALL") == 0){
        printf("Consumer: The line I have just read, didn't pass the requirements.\n");
      }
    }
  }

  else if(strcmp(search_mode, "EQUAL") == 0){
    int flag = (strlen(buff) == L);
    if(flag){
      printf("Consumer: Index: %d, Length: %ld, Line: %s\n", read_index, strlen(buff), buff);
    }
    else{
      if(strcmp(print_mode, "ALL") == 0){
        printf("Consumer: The line I have just read, didn't pass the requirements.\n");
      }
    }
  }

  else{
    int flag = (strlen(buff) > L);
    if(flag){
      printf("Consumer: Index: %d, Length: %ld, Line: %s\n", read_index, strlen(buff), buff);
    }
    else{
      if(strcmp(print_mode, "ALL") == 0){
        printf("Consumer: The line I have just read, didn't pass the requirements.\n");
      }
    }
  }
}

void* cons_routine(void *arg){
  char *buff;
  while(1){
    sem_wait(&taken_slots_sem);
    sem_wait(&buffer_sem);

    buff = buffer[read_index];
    buffer[read_index] = NULL;

    if(strcmp(print_mode, "ALL") == 0){
      printf("Consumer: I have just read line from %d index of buffer. There are %d/%d slots taken.\n", read_index, buffer_slots_taken-1, N);
    }
    cons_print(buff, read_index);

    read_index = (read_index+1)%N;
    buffer_slots_taken--;
    sem_post(&free_slots_sem);
    sem_post(&buffer_sem);
    free(buff);
  }
  if(buff) free(buff);
  return NULL;
}

int main(int argc, char **argv){
  if(argc < 2){
    printf("Wrong number of arguments, try: ./main <config file_path>");
    exit(1);
  }

  load_config(argv[1]);
  atexit(exit_handler);
  signal(SIGINT, int_handler);
  signal(SIGALRM, alrm_handler);
  buffer = malloc(N * sizeof(char *));

  sem_init(&buffer_sem, 0, 1);
  sem_init(&taken_slots_sem, 0, 0);
  sem_init(&free_slots_sem, 0, N);

  pthread_t *prod_array = malloc(sizeof(pthread_t) * P);
  pthread_t *cons_array = malloc(sizeof(pthread_t) * K);
  int i;

  for(i=0; i<P; i++){
    pthread_create(&prod_array[i], NULL, prod_routine, NULL);
  }
  for(i=0; i<K; i++){
    pthread_create(&cons_array[i], NULL, cons_routine, NULL);
  }

  if(nk){
    alarm(nk);
  }

  for(i=0; i<P; i++){
    if(pthread_join(prod_array[i], NULL) != 0){
      printf("Error in pthread_join.\n");
      exit(1);
    }
  }

  if(nk){
    sleep(nk);
  }
  while(1){
    sem_wait(&buffer_sem);
    if (buffer_slots_taken == 0){
      break;
    }
    sem_post(&buffer_sem);
  }

  return 0;
}