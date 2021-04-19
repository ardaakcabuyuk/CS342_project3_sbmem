#include <unistd.h>
#include <stdlib.h>
#include "sbmem.c"
#define ASIZE 256


int main(){

    int i, ret;
    char *p;
    printf("Before sbmem_open\n");
    ret = sbmem_open();
    printf("After sbmem_open\n");
    if (ret == -1)
        exit (1);

    printf("Before sbmem_alloc\n");
    p = sbmem_alloc (ASIZE);
    printf("After sbmem_alloc\n");
    // allocate space to forcharacters
    for (i = 0; i < ASIZE; ++i)
      p[i] = 'a';

    // init all chars to ‘a’

    printf("Before sbmem_alloc\n");
    sbmem_free (p);
    printf("After sbmem_free\n");
    sbmem_close();
    printf("After sbmem_close\n");
    return (0);

}
