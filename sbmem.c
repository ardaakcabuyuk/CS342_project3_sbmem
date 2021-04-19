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
#define MIN_REQUEST 128
#define MAX_REQUEST 4096
#define MIN_MEM 32 * 1024
#define MAX_MEM 256 * 1024

//name of the shared memory
const char* sharedMem = "Shared Segment";

//void pointer that points to shared memory object
void *memPtr;

//tree root to trace the buddy algorithm
MemTree *tree;
struct Pair *root;

//indicates shared mem file
int fd;

//semaphore to protect tree and shared memory
sem_t *semaphore;


// FUNCTIONS USED BY PROGRAMS

/*
*This function will create and initialize shared memory segment
*@param segsize given size for shared memory segment creation, power of 2
*@return 0 if operation is successful, otherwise, 1
*/
int sbmem_init (int segsize){

  int nodeCount = segsize / MIN_REQUEST * 2 - 1;
  int treeSize = nodeCount * sizeof(struct Pair);
  int memSize = 3 * sizeof(int) + treeSize + segsize;
  //number of processes using the library
  int numProcess = 0;

  //delete shared memory if exists
  shm_unlink(sharedMem);

  //create shared memory object
  fd = shm_open(sharedMem, O_CREAT | O_RDWR, 0666);

  if(fd < 0){
    perror("shm_open()");
    return -1;
  }

  semaphore = sem_open(NAME_SEM,O_CREAT | O_EXCL,0666,1);
  sem_unlink(NAME_SEM);


  //arrange the size of shared memory Segment
  ftruncate(fd, memSize);

  //pointer ptrSharred maps the beginning of the shared memory Segment
  void *ptrShared = mmap(NULL,memSize,PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);


  if(ptrShared == MAP_FAILED){
      perror("mmap err");
      return -1;
  }

  int *sizes = (int*) ptrShared;

  sizes[0] = memSize;
  sizes[1] = treeSize;
  sizes[2] = numProcess;

  //create the tree to trace the buddy algorithm
  tree = (MemTree *) (3 * sizeof(int) + (char*) ptrShared);
  root = (struct Pair *) (3 * sizeof(int) + (char*) ptrShared + sizeof(MemTree));
  tree->root = root;
  createMemTree(root, segsize);

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

    semaphore = sem_open(NAME_SEM, O_CREAT,0660,1);
    int fileDescriptor = shm_open(sharedMem, O_RDWR, 0666);

    void *ptrShared = mmap(NULL, 3 * sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fileDescriptor,0);
    if(((int *) ptrShared)[2] >= 10){
      printf("Library can not be used. There are too many processes\n" );
      return -1;
    }
    int *sizePtr = (int*) ptrShared;
    int sizeMem = *sizePtr;
    void *whole_mem = mmap(NULL, sizeMem, PROT_READ|PROT_WRITE, MAP_SHARED, fileDescriptor,0);

    memPtr = (char *) whole_mem + 3 * sizeof(int) + sizeof(MemTree) + ((int *) whole_mem)[1];
    tree = (MemTree *) (3 * sizeof(int) + (char*) whole_mem);
    root = (struct Pair *) (3 * sizeof(int) + (char*) whole_mem + sizeof(MemTree));
    tree->root = root;

    //char * char_mem = (char*) mem_shared;
    sem_wait(semaphore);
    ((int *) whole_mem)[2] = ((int *) whole_mem)[2] + 1;
    printf("\nHi, process %d\n", getpid());
    printf("NUMBER OF PROCESSES = %d\n", ((int *) whole_mem)[2]);
    sem_post(semaphore);
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
  struct Pair *block;
  int success = 0;

  printf("\nRequest by process %d of size %d\n", getpid(), reqsize);

  sem_wait(semaphore);
  findBlock(tree, root, actualSize, &block, &success);
  sem_post(semaphore);

  if (success) {
    block->fragmentation = internal_fragmentation;
    printf("\nProcess %d allocated (%d, %d)\n", getpid(), block->start, block->end);
    printf("with internal fragmentation: %d\n", block->fragmentation);

    //sem_wait(semaphore);
    intptr_t start_address = (intptr_t) memPtr + block->start;
    //sem_post(semaphore);

    return (void *) start_address;
  }
  else {
    printf("Cannot allocate the request by process %d of size %d.\n", getpid(), reqsize);
    return NULL;
  }

}

/*
*This function deallocates the memory space that is pointed by ptr
*@param ptr pointer pointinf the memory space allocated earlier
*
*/
void sbmem_free(void *ptr) {
  int treeStart = (char *) ptr - (char *) memPtr;
  printf("\nDeallocate address %d\n", treeStart);
  sem_wait(semaphore);
  deallocate(tree, treeStart);
  merge(tree, root);
  sem_post(semaphore);
}

/*
*Shared memory space will be unmapped when process using that memory
*when process done with the library using this memory space
*
*
*/
int sbmem_close() {
  printf("\nProcess %d out.\n", getpid());
  ((int *) tree)[-1] = ((int *) tree)[-1] - 1;
  printf("NUMBER OF PROCESSES = %d\n", ((int *) tree)[-1]);
  unlink(sharedMem);
  return 1;
}
