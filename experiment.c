#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "sbmem.c"
#define SETNO 1

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
    char *p;
    for (int i = 0; i < 8; i++) {
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

          sbmem_close();
          exit(0);
        }
    }
    while(wait(NULL) > 0);
    char *commandArray[2];
    char *command = "./destroy_memory";
    commandArray[0] = command;
    commandArray[1] = NULL;
    execvp(*commandArray, commandArray);
    printf("destroy_memory executed.\n");
    return (0);
}
