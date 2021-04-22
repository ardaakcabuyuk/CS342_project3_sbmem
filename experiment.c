#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "sbmem.c"
#define SETNO 5
#define PROCESS_COUNT 8

void *allocate(int size) {
  char *p = sbmem_alloc (size);
  if (p != NULL) {
    // allocate space to forcharacters
    for (int i = 0; i < size; ++i) {
      p[i] = 'a';
    }
  }
  return p;
}

void dealloc(void *p) {
  if (p != NULL) {
    sbmem_free(p);
  }
}


int main(){
    srand(time(NULL));
    int i, ret, aSize;
    int fd[2];
    if (pipe(fd) == -1) {
      fprintf(stderr,"Pipe failed");
      return 1;
    }
    int overall_fragmentation = 0;
    write(fd[1], &overall_fragmentation, sizeof(overall_fragmentation));
    char *p;
    for (int i = 0; i < PROCESS_COUNT; i++) {
      int pid = fork();
      if (pid == 0) {
          ret = sbmem_open();
          if (ret == -1)
              exit (1);

          char *a1 = allocate(128 * SETNO);
          char *a2 = allocate(150 * SETNO);
          dealloc(a1);
          char *a3 = allocate(200 * SETNO);
          dealloc(a3);
          char *a4 = allocate(300 * SETNO);
          char *a5 = allocate(400 * SETNO);
          dealloc(a5);
          dealloc(a4);
          dealloc(a2);
          char *a6 = allocate(256 * SETNO);
          dealloc(a6);

          int total_fragmentation = sbmem_close();
          int overall_fragmentation;
          read(fd[0], &overall_fragmentation, sizeof(overall_fragmentation));
          overall_fragmentation += total_fragmentation;
          write(fd[1], &overall_fragmentation, sizeof(overall_fragmentation));
          exit(0);
        }
    }
    while(wait(NULL) > 0);
    read(fd[0], &overall_fragmentation, sizeof(overall_fragmentation));
    printf("Overall fragmentation for %d processes = %d bytes.\n", PROCESS_COUNT, overall_fragmentation);
    char *commandArray[2];
    char *command = "./destroy_memory";
    commandArray[0] = command;
    commandArray[1] = NULL;
    execvp(*commandArray, commandArray);
    printf("destroy_memory executed.\n");
    return (0);
}
