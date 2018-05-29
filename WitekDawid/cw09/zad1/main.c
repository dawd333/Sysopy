#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

int P, K, N, L, nk;
char *search_mode, *print_mode;
FILE *source_file;
char** buffer = NULL;
int write_index=0, read_index=0, buffer_slots_taken=0, can_exit=0, cons_exit_count=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;

void exit_handler(){
  if (buffer) free(buffer);
  if (source_file) fclose(source_file);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&empty_cond);
  pthread_cond_destroy(&full_cond);
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
  nk = (int) strtol(tmp, NULL, 10); //bez ifa bo strtol ma problemy z 0

  fclose(file);
}

void* prod_routine(void *arg){
  char *buff = NULL;
  size_t n = 0;
  while(1){
    pthread_mutex_lock(&mutex);
    while(buffer_slots_taken >= N){
      pthread_cond_wait(&full_cond, &mutex);
    }
    if(getline(&buff, &n, source_file) == -1){
      pthread_mutex_unlock(&mutex);
      break;
    }
    if(strcmp(print_mode, "ALL") == 0){
      printf("Producer: I have just put line into %d index of buffer. There are %d/%d slots taken.\n", write_index, buffer_slots_taken+1, N);
    } 
    buffer[write_index] = buff;
    write_index = (write_index+1)%N;
    buffer_slots_taken++;
    pthread_cond_signal(&empty_cond);
    pthread_mutex_unlock(&mutex);
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
    pthread_mutex_lock(&mutex);
    while(buffer_slots_taken <= 0){
      if(can_exit){
        cons_exit_count++;
        return NULL;
      }
      pthread_cond_wait(&empty_cond, &mutex);
    }

    buff = buffer[read_index];
    buffer[read_index] = NULL;

    if(strcmp(print_mode, "ALL") == 0){
      printf("Consumer: I have just read line from %d index of buffer. There are %d/%d slots taken.\n", read_index, buffer_slots_taken-1, N);
    }
    cons_print(buff, read_index);

    read_index = (read_index+1)%N;
    buffer_slots_taken--;
    pthread_cond_signal(&full_cond);
    pthread_mutex_unlock(&mutex);
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

  can_exit = 1;

  while(1){
    pthread_mutex_lock(&mutex);
    if(buffer_slots_taken == 0){
      while(cons_exit_count != K){
        pthread_cond_signal(&empty_cond);
        pthread_mutex_unlock(&mutex);
      }
      if(nk){
        sleep(nk);
      }
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);
  }

  return 0;
}