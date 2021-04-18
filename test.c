#include "MemTree.c"
#include <stdio.h>
#include <math.h>

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

  found->allocated = 0;
  merge(tree, tree->root);

  findBlock(tree, tree->root, 32, &found);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  findBlock(tree, tree->root, 2, &found);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  struct Pair *first2 = found;

  findBlock(tree, tree->root, 2, &found);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  first2->allocated = 0;
  merge(tree, tree->root);

  found->allocated = 0;
  merge(tree, tree->root);

  findBlock(tree, tree->root, 64, &found);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);
  return 0;
}
