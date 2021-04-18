#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include "MemTree.c"

//size of shared memory segment
int sharedSize;

//name of the shared memory
const char* sharedMem = "Shared Segment"

//void pointer that points to shared memory object
void *ptrShared;

//tree to trace the buddy algorithm
MemTree *tree;

//indicates shared mem file
int fd;

//semaphore to protect tree and shared memory
semaphore treeSemaphore = 1;
semaphore memSemaphore = 1;

// FUNCTIONS USED BY PROGRAMS

/*
*This function will create and initialize shared memory segment
*@param segsize given size for shared memory segment creation, power of 2
*@return 0 if operation is successful, otherwise, 1
*/
int sbmem_init (int segsize){

    sharedSize = segSize;

    //delete shared memory if exists
    shm_unlink(sharedMem);

    //create shared memory object
    fd = shm_open(sharedMem, O_CREAT | O_RDWR, 0666);
    if(fd < 0){
      perror("shm_open()");
      exit(1);
    }

    //pointer ptrSharred maps the beginning of the shared memory Segment
    ptrShared = mmap(0,sharedSize,PROT_READ, MAP_SHARED, fd, 0);

    //arrange the size of shared memory Segment
    ftruncate(fd, segSize);

    //create the tree to trace the buddy algorithm
    tree = createMemTree(sharedSize);
}


/*
*This function removes the shared memory segment
*from the system and it will do all cleanup
*/
void sbmem_remove(){

    //do necessary cleanups here


    //remove shared memory segment from the memory
    shm_unlink(sharedMem);
}

//FUNCTIONS USED BY OTHER PROCESSES

/*
*If a process need to use library, it calls this function
*Map sharing segment to logical address of process by mmap()
*@return -1 if too many processes are using library
*@return 0, if process is able to use library
*/
int sbmem_open() {

}

/*
*This function allocates memory of size n (in bytes)
*n is next power of 2 that corresponds reqsize
*@param reqsize required size that will be allocated
*@return pointer to allocated space
*NULL pointer is there is not enough memory
*/
void *sbmem_alloc(int reqsize) {
  int actualSize = pow(2, (int) ceil(log2(reqsize)));
  int internal_fragmentation = actualSize - reqsize;
  struct Pair *block;
  int success = 0;

  wait(treeSemaphore);
  findBlock(tree, tree->root, actualSize, &block, &success);
  signal(treeSemaphore);

  if (success) {
    block->fragmentation = internal_fragmentation;
    printf("Allocated:\n");
    printf("Start: %d\n", block->start);
    printf("End: %d\n", block->end);
    printf("internal fragmentation: %d\n", block->fragmentation);

    wait(memSemaphore);
    int start_address = ptrShared + block->start;
    printf("Shared Memory Start Address: %d\n", ptrShared);
    printf("Start Address: %d\n", ptrShared + block->start);
    signal(memSemaphore);

    return start_address;
  }
  else {
    printf("Cannot allocate.\n");
    return NULL;
  }

}

/*
*This function deallocates the memory space that is pointed by ptr
*@param ptr pointer pointinf the memory space allocated earlier
*
*/
void sbmem_free(void *ptr) {

}

/*
*Shared memory space will be unmapped when process using that memory
*when process done with the library using this memory space
*
*
*/
int sbmem_close() {

}
