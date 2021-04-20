#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <semaphore.h>
#include "sbmem.c"

int main() {
  sbmem_init(pow(2, (int) ceil(log2(MIN_MEM + rand() % (MAX_MEM- MIN_MEM + 1)))));
  while(1);
  return 0;
}
