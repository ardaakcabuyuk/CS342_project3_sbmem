#include "MemTree.c"
#include <stdio.h>
#include <math.h>

int main() {
  MemTree *tree = createMemTree(128);
  printf("Created.\n");
  struct Pair *found;
  int success = 0;
  findBlock(tree, tree->root, 32, &found, &success);
  printf("start = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  findBlock(tree, tree->root, 8, &found, &success);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  deallocate(tree, found->start);
  merge(tree, tree->root);

  findBlock(tree, tree->root, 32, &found, &success);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  findBlock(tree, tree->root, 2, &found, &success);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  struct Pair *first2 = found;

  findBlock(tree, tree->root, 2, &found, &success);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);

  deallocate(tree, first2->start);
  merge(tree, tree->root);

  deallocate(tree, found->start);
  merge(tree, tree->root);

  findBlock(tree, tree->root, 64, &found, &success);
  printf("\nstart = %d\n", found->start);
  printf("end = %d\n", found->end);
  printf("size = %d\n", found->size);
  printf("allocated? = %d\n", found->allocated);
  return 0;
}
