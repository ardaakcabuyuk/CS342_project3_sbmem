#include <unistd.h>
#include <stdlib.h>
#include "sbmem.c"
#define ASIZE 64


int main(){
    int i, ret;
    char *p;
    int pid = fork();
    if (pid == 0) {
      pid = fork();
      if (pid == 0) {
        ret = sbmem_open();
        if (ret == -1)
            exit (1);

        p = sbmem_alloc (8);
        if (p != NULL) {
          // allocate space to forcharacters
          for (i = 0; i < 8; ++i) {
            p[i] = 'a';
          }
          sbmem_free (p);
        }

        p = sbmem_alloc (16);
        if (p != NULL) {
          // allocate space to forcharacters
          for (i = 0; i < 16; ++i) {
            p[i] = 'a';
          }
          sbmem_free (p);
        }

        sbmem_close();
        exit(0);
      }
      ret = sbmem_open();
      if (ret == -1)
          exit (1);

      p = sbmem_alloc (13);
      if (p != NULL) {
        // allocate space to forcharacters
        for (i = 0; i < 8; ++i) {
          p[i] = 'a';
        }
        sbmem_free (p);
      }

      p = sbmem_alloc (32);
      if (p != NULL) {
        // allocate space to forcharacters
        for (i = 0; i < 32; ++i) {
          p[i] = 'a';
        }
        sbmem_free (p);
      }

      /*printf("Before sbmem_alloc\n");
      p = sbmem_alloc (32);
      if (p != NULL) {
        printf("After sbmem_alloc\n");
        // allocate space to forcharacters
        for (i = 0; i < 32; ++i) {
          p[i] = 'a';
          printf("p[%d] = %c\n", i, p[i]);
        }
        printf("Before sbmem_free\n");
        sbmem_free (p);
        printf("After sbmem_free\n");
      }*/

      sbmem_close();
      exit(0);
    }

    ret = sbmem_open();
    if (ret == -1)
        exit (1);

    p = sbmem_alloc (10);
    if (p != NULL) {
      // allocate space to forcharacters
      for (i = 0; i < 10; ++i) {
        p[i] = 'a';
      }
      sbmem_free (p);
    }

    p = sbmem_alloc (ASIZE);
    if (p != NULL) {
      // allocate space to forcharacters
      for (i = 0; i < ASIZE; ++i) {
        p[i] = 'a';
      }

      sbmem_free (p);
    }
    wait(NULL);
    char *commandArray[2];
    char *command = "./destroy_memory";
    commandArray[0] = command;
    commandArray[1] = NULL;
    execvp(*commandArray, commandArray);
    printf("destroy_memory executed.\n");
    return (0);
}
