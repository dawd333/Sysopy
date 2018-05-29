#ifndef ZAD2_SPEC_H
#define ZAD2_SPEC_H
#define MAXCLIENTS 1000

typedef struct shmstruct{
  int seat_limit;
  int client_counter;
  int asleep;
  int currently_shaved;
  int seats[MAXCLIENTS];
}Shm;

const char *semaphore_path = "./";
const char *shm_path = "/SHM";
const char *seatsarray = "/SEATSARRAYSEM";
const char *asleep = "/ASLEEPSEM";
const char *wakeup = "/WAKEUPSEM";
const char *sit = "/SITSEM";
const char *shaveme = "/SHAVEMESEM";
const char *shavingended = "/SHAVINGENDEDSEM";
const char *ileft = "/ILEFTSEM";

#endif