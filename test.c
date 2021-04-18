#include "MemTree.c"
#include <stdio.h>

int main() {
  MemTree *tree = createMemTree(128);
  printf("Created.\n");
  struct Pair *found;
  findBlock(tree, tree->root, 32, &found);
  printf("start = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  findBlock(tree, tree->root, 8, &found);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  findBlock(tree, tree->root, 64, &found);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);
  return 0;
}
