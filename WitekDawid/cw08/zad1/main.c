#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <sys/times.h>

int max(int a, int b){
  if(a>b) return a;
  else return b;
}

int min(int a, int b){
  if(a<b) return a;
  else return b;
}

double time_difference(clock_t start, clock_t end){
  return (double)(end - start)/ sysconf(_SC_CLK_TCK);
}

int **I; //initial picture
float **K; //filter
int **J; //picture at the end

int W,H,M,c;
int threads;

void load_picture_matrix(char *file_path){
  FILE *file;
  if((file = fopen(file_path, "r")) == NULL){
    printf("Couldn't open file with picture.\n");
    exit(1);
  }

  fscanf(file, "P2\n");
  fscanf(file, "%d", &W);
  fscanf(file, "%d", &H);
  fscanf(file, "%d", &M); //wczytywanie zmiennych z poczatku pliku
  
  I = malloc(sizeof(int*) * H); //tab[wiersze = y][kolumny = x]
  for(int i=0; i<H; i++){
    I[i] = malloc(sizeof(int) * W);
  }

  for(int y=0; y<H; y++){
    for(int x=0; x<W; x++){
      fscanf(file, "%d", &I[y][x]);
    }
  }

  fclose(file);
}

void load_filter_matrix(char *file_path){
  FILE *file;
  if((file = fopen(file_path, "r")) == NULL){
    printf("Couldn't open file with filter.\n");
    exit(1);
  }

  fscanf(file, "%d", &c); //wczytywanie zmiennej z poczatku pliku
  K = malloc(sizeof(float*) * c); //tab[wiersze = y][kolumny = x]
  for(int i=0; i<c; i++){
    K[i] = malloc(sizeof(float) * c);
  }

  for(int y=0; y<c; y++){
    for(int x=0; x<c; x++){
      fscanf(file, "%f", &K[y][x]);
    }
  }

  fclose(file);
}

void save_result_matrix(char *file_path){
  FILE *file;
  if((file = fopen(file_path, "w")) == NULL){
    printf("Couldn't open file with result.\n");
    exit(1);
  }
  fprintf(file, "P2\n%d %d\n%d\n", W, H, M); //wpisanie poczatkowych zmiennych
  for(int y=0; y<H; y++){
    for(int x=0; x<W; x++){
      fprintf(file, "%d ", J[y][x]);
    }
    fprintf(file, "\n"); //znaki konca linii po kazdym wierszu
  }

  fclose(file);
}

void save_times(char *picture_file_path, char *filter_file_path, clock_t real_time[2], struct tms tms_time[2]){
  FILE *file;
  if((file = fopen("Times.txt", "a")) == NULL){
    printf("Couldn't open file with time results.\n");
    exit(1);
  }
  fprintf(file, "Picture: %s, picture size: %dx%d\n", picture_file_path, W, H);
  fprintf(file, "Filter: %s, filter size: %dx%d\n", filter_file_path, c, c);
  fprintf(file, "Number of threads: %d\n", threads);
  fprintf(file, "Real: %.3lfs    User: %.3lfs    System: %.3lfs\n", time_difference(real_time[0], real_time[1]), time_difference(tms_time[0].tms_utime, tms_time[1].tms_utime), time_difference(tms_time[0].tms_stime, tms_time[1].tms_stime));
  fprintf(file, "----------------------------------------------\n");

  fclose(file);
}

void* start_routine(void *arg){
  int c2 = (int) ceil(c/2);
  int thread_number = *(int *) arg;
  double s;
  int start_x = W * thread_number / threads;
  int end_x = W * (thread_number+1) / threads;
  for(int x=start_x; x<end_x; x++){
    for(int y=0; y<H; y++){
      s=0;
      for(int w=0; w<c; w++){
        for(int h=0; h<c; h++){
          int xi = min((W-1), max(0, x-c2+w));
          int yi = min((H-1), max(0, y-c2+h)); //uwzglednienie wypadniecia poza macierz
          s+= I[yi][xi] * K[h][w];
          //printf("Thread number: %d, x: %d, y: %d, w: %d, h: %d, xi: %d, yi: %d, s: %f\n", thread_number, x, y, w, h, xi, yi, s);
        }
      }
      J[y][x] = (int) round(s);
    }
  }
  return NULL;
}

int main(int argc, char **argv){
  if(argc != 5){
    printf("Arguments should be: ./main, number of threads, picture file path, filter file path, result picture file path.\n");
    exit(1);
  }

  if((threads = (int) strtol(argv[1], NULL, 10)) <= 0){
    printf("Number of threads have to be greater than 0\n");
    exit(1);
  }
  char* picture_file_path = argv[2];
  char* filter_file_path = argv[3];
  char* result_file_path = argv[4];
  clock_t real_time[2];
  struct tms tms_time[2];

  load_picture_matrix(picture_file_path);
  load_filter_matrix(filter_file_path);

  J = malloc(sizeof(int *) * H);
  for(int i=0; i<H; i++){
    J[i] = malloc(sizeof(int) * W); //zaalokowanie pamieci na rezultat
  }

  pthread_t *thread_array = malloc(sizeof(pthread_t) * threads); //stworzenie tablicy watkow

  real_time[0] = times(&tms_time[0]);

  for(int i=0; i<threads; i++){
    int* arg = (int *) malloc(sizeof(int));
    *arg = i; //potrzebuje pointera do pthread_create arg
    pthread_create(&thread_array[i], NULL, start_routine, arg); //tworzenie watku
  }
  for(int i=0; i<threads; i++){
    void *x;
    pthread_join(thread_array[i], &x); //czekanie na zakonczenie watku i zakonczenie
  }

  real_time[1] = times(&tms_time[1]);

  save_result_matrix(result_file_path);
  save_times(picture_file_path, filter_file_path, real_time, tms_time);

  return 0;
}