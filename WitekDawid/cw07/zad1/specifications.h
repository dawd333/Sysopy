#ifndef ZAD1_SPEC_H
#define ZAD1_SPEC_H
#define MAXCLIENTS 1000
#define SEATSARRAYSEM 0
#define ASLEEPSEM 1
#define WAKEUPSEM 2
#define SITSEM 3
#define SHAVEMESEM 4
#define SHAVINGENDEDSEM 5
#define ILEFTSEM 6
#define PROJECTID 2018

typedef struct shmstruct{
  int seat_limit;
  int client_counter;
  int asleep;
  int currently_shaved;
  int seats[MAXCLIENTS];
}Shm;

const char *semaphore_path = "./";
const char *shm_path = "/SHM";

#endif