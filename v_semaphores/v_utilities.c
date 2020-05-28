#include "v_utilities.h"

void end_program(const char *msg) {
  perror(msg);
  exit(1);
}

void lock(int semid, int sem_num) { 
  struct sembuf semb;

  semb.sem_num = sem_num;
  semb.sem_op = -1;
  semb.sem_flg = SEM_UNDO;
  if (semop(semid, &semb, 1) == -1) {
    printf("[FAILED] %d\n", sem_num);
    end_program("semop lock");
  }

}

void unlock(int semid, int sem_num) {
  struct sembuf semb;

  semb.sem_num = sem_num;
  semb.sem_op = 1;
  semb.sem_flg = SEM_UNDO;
  if (semop(semid, &semb, 1) == -1) {
    end_program("semop unlock");
  }

}

void w_shoot(int semid, int fd, char *buf) {
    lock(semid, EMPT);
    lock(semid, MUTX);
    write(fd, buf, sizeof(buf));
    memset(buf, 0, strlen(buf));
    unlock(semid, MUTX);
    unlock(semid, FULL);
}

int init_v_system(const char* f_name, int buffer_size){

  key_t key;
  int semid;
  union semun sop;
  unsigned short array[3];
  array[MUTX] = 1;
  array[FULL] = 0;
  array[EMPT] = buffer_size;
  sop.array = array;
  // fprintf(stdout, "%d\n", sizeof(BUFFER));  

  if ((key = ftok(f_name, 'O')) == -1) { 
    end_program("ftok");
  }

  /* create a semaphore set with 1 semaphore: */
  if ((semid = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
    end_program("semget");
  }

  #ifdef DEBUG
  fprintf(stdout, "%d\n", semid);
  #endif

  /* initialize semaphore #0 to 1: */
  if (semctl(semid, 0, SETALL, sop) == -1) {
    end_program("semctl");
  }

  return semid;
}