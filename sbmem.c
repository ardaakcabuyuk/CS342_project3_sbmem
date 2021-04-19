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
#include "MemTree.c"

#define NAME_SEM "semaphore"
//size of shared memory segment
int sharedSize;

//name of the shared memory
const char* sharedMem = "Shared Segment";

//void pointer that points to shared memory object
void *ptrShared;

//tree to trace the buddy algorithm
MemTree *tree;

//indicates shared mem file
int fd;

//semaphore to protect tree and shared memory
sem_t *semaphore;


//number of processes using the library
int numProcess = 0;

// FUNCTIONS USED BY PROGRAMS

/*
*This function will create and initialize shared memory segment
*@param segsize given size for shared memory segment creation, power of 2
*@return 0 if operation is successful, otherwise, 1
*/
int sbmem_init (int segsize){

    sharedSize = segsize;

    printf("x0\n" );
    //delete shared memory if exists
    shm_unlink(sharedMem);
    printf("x1\n" );
    //create shared memory object
    fd = shm_open(sharedMem, O_CREAT | O_RDWR, 0666);
    printf("x2\n" );
    if(fd < 0){
      perror("shm_open()");
      return -1;
    }
    printf("x3\n" );
    semaphore = sem_open(NAME_SEM,O_CREAT | O_EXCL,0666,1);
    sem_unlink(NAME_SEM);
    printf("x4\n" );

    //arrange the size of shared memory Segment
    ftruncate(fd, segsize);
    printf("x5\n" );
    //pointer ptrSharred maps the beginning of the shared memory Segment
    ptrShared = mmap(NULL,sharedSize,PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    printf("x6\n" );

    if(ptrShared == MAP_FAILED){
        perror("mmap err");
        return -1;
    }
    printf("x7\n" );
    int *size = (int*) ptrShared;
    printf("x8\n" );
    size[0] = sharedSize;
    printf("x9\n" );

    //create the tree to trace the buddy algorithm
    printf("a0\n" );
    tree = createMemTree(sharedSize);
    printf("a1\n" );
    return 0;

}


/*
*This function removes the shared memory segment
*from the system and it will do all cleanup
*/
void sbmem_remove(){

    //do necessary cleanups here
    sem_destroy(semaphore);

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

    if(numProcess >= 10){
      printf("Library can not be used. There are too many processes\n" );
      return -1;
    }
    semaphore = sem_open(NAME_SEM, O_CREAT,0660,1);
    int fileDescriptor = shm_open(sharedMem, O_RDWR, 0666);

    void *sharedPtr = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fileDescriptor,0);

    int *sizePtr = (int*) sharedPtr;

    int sizeMem = *sizePtr;

    void * mem_shared = mmap(NULL, sizeMem, PROT_READ|PROT_WRITE, MAP_SHARED, fileDescriptor,0);

    //char * char_mem = (char*) mem_shared;
    numProcess++;



    return 0;
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
  printf("test0\n" );
  struct Pair *block;
  printf("test1\n" );
  int success = 0;


  sem_wait(semaphore);
  printf("test2\n" );
  findBlock(tree, tree->root, actualSize, &block, &success);
  printf("test3\n" );
  sem_post(semaphore);
  printf("test4\n" );

  if (success) {
    block->fragmentation = internal_fragmentation;
    printf("Allocated:\n");
    printf("PID: %d\n", getpid());
    printf("Start: %d\n", block->start);
    printf("End: %d\n", block->end);
    printf("internal fragmentation: %d\n", block->fragmentation);

    sem_wait(semaphore);
    intptr_t start_address = (intptr_t) ptrShared + block->start;
    printf("Shared Memory Start Address: %ld\n", (intptr_t) ptrShared);
    printf("Start Address: %ld\n", start_address);
    sem_post(semaphore);

    return (void *) start_address;
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
  int treeStart = ptr - (void *) sharedMem;
  sem_wait(semaphore);
  deallocate(tree, treeStart);
  merge(tree, tree->root);
  sem_post(semaphore);
}

/*
*Shared memory space will be unmapped when process using that memory
*when process done with the library using this memory space
*
*
*/
int sbmem_close() {

}
