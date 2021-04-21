#include "MemTree.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
int blockFound = 0;

void createMemTree(struct Pair *rootNode, int sizeMem) {
  rootNode->start = 0;
  rootNode->end = sizeMem - 1;
  rootNode->size = sizeMem;
  rootNode->left = NULL;
  rootNode->right = NULL;
  rootNode->allocated = 0;
  rootNode->fragmentation = -1;
  rootNode->depth = 0;
  rootNode->horizontal = 0;
}

void destruct(struct Pair *node) {
  if (node != NULL) {
    destruct(node->left);
    destruct(node->right);
    node = NULL;
  }
}

void findBlock(MemTree *tree, struct Pair *node, int size, struct Pair **block, int *success) {
  if (node == tree->root)
    blockFound = 0;

  printf("\nSEARCH size %d for pid = %d\n", size, getpid());
  printf("start = %d\n", node->start);
  printf("end = %d\n", node->end);
  printf("size = %d\n", node->size);
  printf("allocated? = %d\n", node->allocated);
  if (!blockFound) {
    if (node != NULL) {
      /*printf("\nstart = %d\n", node->start);
      printf("end = %d\n", node->end);
      printf("size = %d\n", node->size);
      printf("allocated? = %d\n", node->allocated);*/
      if (node->left == NULL && size < node->size && !(node->allocated)) {
        while (size < node->size) {
          split(tree, node);
          printf("SPLIT LOOPU\n");
          node = node->left;
        }
        node->allocated = 1;
        blockFound = 1;
        *success = 1;
        *block = node;
      }
      else if (node->left == NULL && size == node->size && !(node->allocated)) {
        node->allocated = 1;
        blockFound = 1;
        *success = 1;
      	*block = node;
      }
      else if (size < node->size) {
        printf("size = %d\n", size);
        printf("node size = %d\n", node->size);
      	findBlock(tree, node->left, size, block, success);
      	findBlock(tree, node->right, size, block, success);
      }
    }
  }


}

void split(MemTree *tree, struct Pair *node) {

    int newSize = node->size / 2;
    int leftStart = node->start;
    int rightStart = leftStart + newSize;
    int leftEnd = leftStart + newSize - 1;
    int rightEnd = rightStart + newSize - 1;
    int depth = node->depth + 1;
    int horizontalLeft = 2 * node->horizontal;
    int horizontalRight = horizontalLeft + 1;
    printf("split node (%d, %d) with depth = %d into (%d, %d) and (%d, %d) with depths = %d\n", node->start, node->end, depth - 1, leftStart, leftEnd, rightStart, rightEnd, depth);

    printf("1\n");
    printf("size = %ld\n", sizeof(struct Pair));
    node->left = (struct Pair *) (node + ((int) pow(2, node->depth) + node->horizontal));
    printf("node = %p\n", node);
    printf("node->left = %p\n", node->left);
    printf("2\n");
    node->left->size = newSize;
    printf("2 1\n");
    node->left->start = leftStart;
    printf("2 2\n");
    node->left->end = leftEnd;
    printf("2 3\n");
    node->left->left = NULL;
    printf("2 4\n");
    node->left->right = NULL;
    printf("2 5\n");
    node->left->allocated = 0;
    printf("2 6\n");
    node->left->fragmentation = -1;
    printf("2 7\n");
    node->left->depth = depth;
    printf("2 8\n");
    node->left->horizontal = horizontalLeft;

    printf("3\n");
    node->right = (struct Pair *) (node + ((int) pow(2, node->depth) + node->horizontal + 1));
    printf("4\n");
    node->right->size = newSize;
    node->right->start = rightStart;
    node->right->end = rightEnd;
    node->right->left = NULL;
    node->right->right = NULL;
    node->right->allocated = 0;
    node->right->fragmentation = -1;
    node->right->depth = depth;
    node->right->horizontal = horizontalRight;

    printf("SPLIT son\n");
}

void merge(MemTree *tree, struct Pair *node) {
  if (node->left != NULL) {
    merge(tree, node->left);
    if (node->left->allocated == 0 && node->right->allocated == 0 && (node->left->left == NULL && node->right->left == NULL)) {
      printf("merge node (%d, %d) and (%d, %d) into (%d, %d)\n", node->left->start, node->left->end, node->left->end + 1, node->end, node->start, node->end);
      node->left = NULL;
      node->right = NULL;
    }
    if (node->right != NULL)
      merge(tree, node->right);
	}
}

void deallocate(MemTree *tree, int startAddress) {
  struct Pair *cur = tree->root;
  while (cur->left != NULL) {
    if (startAddress >= cur->start + cur->size / 2) {
      cur = cur->right;
    }
    else {
      cur = cur->left;
    }
  }
  if (cur->start == startAddress && cur->allocated) {
    printf("Process %d deallocates (%d, %d)\n", getpid(), cur->start, cur->end);
    cur->allocated = 0;
  }

}
