#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include "sbmem.c"
#define ASIZE 64


int main(){
    srand(time(NULL));
    sbmem_init(pow(2, (int) ceil(log2(MIN_MEM + rand() % (MAX_MEM- MIN_MEM + 1)))));
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
    return (0);
}
