#include "MemTree.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int blockFound = 0;

void createMemTree(struct Pair *rootNode, int sizeMem) {
  rootNode->start = 0;
  rootNode->end = sizeMem - 1;
  rootNode->size = sizeMem;
  rootNode->left = NULL;
  rootNode->right = NULL;
  rootNode->allocated = 0;
  rootNode->fragmentation = -1;
}

void destruct(struct Pair *node) {
  if (node != NULL) {
    destruct(node->left);
    destruct(node->right);
    free(node);
    node = NULL;
  }
}

void findBlock(MemTree *tree, struct Pair *node, int size, struct Pair **block, int *success) {
  if (node == tree->root)
    blockFound = 0;

  if (!blockFound) {
    if (node != NULL) {
      /*printf("\nstart = %d\n", node->start);
      printf("end = %d\n", node->end);
      printf("size = %d\n", node->size);
      printf("allocated? = %d\n", node->allocated);*/
      if (node->left == NULL && size < node->size && !node->allocated) {
        while (size < node->size) {
          split(tree, node);
          node = node->left;
        }
        node->allocated = 1;
        blockFound = 1;
        *success = 1;
        *block = node;
      }
      else if (node->left == NULL && size == node->size && !node->allocated) {
        node->allocated = 1;
        blockFound = 1;
        *success = 1;
      	*block = node;
      }
      else if (size < node->size) {
        //printf("else if");
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
    printf("split node (%d, %d) into (%d, %d) and (%d, %d)\n", node->start, node->end, leftStart, leftEnd, rightStart, rightEnd);

    node->left = (struct Pair *) (node + sizeof(struct Pair));
    node->left->size = newSize;
    node->left->start = leftStart;
    node->left->end = leftEnd;
    node->left->left = NULL;
    node->left->right = NULL;
    node->left->allocated = 0;
    node->left->fragmentation = -1;

    node->right = (struct Pair *) (node + 2 * sizeof(struct Pair));
    node->right->size = newSize;
    node->right->start = rightStart;
    node->right->end = rightEnd;
    node->right->left = NULL;
    node->right->right = NULL;
    node->right->allocated = 0;
    node->right->fragmentation = -1;
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
