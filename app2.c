#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "sbmemlib.c"
#define ASIZE 64


int main(){
    srand(time(NULL));
    int i, ret, aSize;
    char *p;
    for (int i = 0; i < 50; i++) {
      int pid = fork();
      if (pid == 0) {
          ret = sbmem_open();
          if (ret == -1)
              exit (1);

          for (int i = 0; i < 10; i++) {
            aSize = MIN_REQUEST + rand() % (MAX_REQUEST - MIN_REQUEST + 1);
            printf("aSize %d\n", aSize );
            p = sbmem_alloc (aSize);
            if (p != NULL) {
              // allocate space to forcharacters
              for (i = 0; i < aSize; ++i) {
                p[i] = 'a';
              }
              sbmem_free (p);
            }
          }

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
