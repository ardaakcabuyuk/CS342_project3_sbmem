#include "ReadyQueue.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

MemTree *createMemTree(int sizeMem) {
  MemTree *tree = (MemTree *) malloc(sizeof(MemTree));

  //initialize root node (i.e. whole memory)
  struct Pair *rootNode = (struct Pair *) malloc(sizeof(struct Pair));
  rootNode->start = 0;
  rootNode->end = sizeMem - 1;
  rootNode->size = sizeMem;

  tree->root = rootNode;
  return tree;
}

void destruct(struct Pair *node) {
  if (node != NULL) {
    destruct(node->left);
    destruct(node->right);
    free(node);
    node = NULL;
  }
}

struct Pair *findBlock(MemTree *tree, struct Pair *node, int size) {
  if (node->left == NULL && size < node->size) {
    while (size < node->size) {
      split(tree, node);
      node = node->left;
    }
  }
  else if (size == node->size && !node->allocated)
  	return node;
  else if (size < node->size)
  	findBlock(node->left, size);
  else
  	findBlock(node->right, size);
}

struct Pair *findBlock(MemTree *tree, struct Pair *node, int size) {
  if (node->left == NULL && size < node->size) {
    while (size < node->size) {
      split(tree, node);
      node = node->left;
    }
    return node;
  }
  else if (size == node->size && !node->allocated)
  	return node;
  else if (size < node->size)
  	findBlock(node->left, size);
  else
  	findBlock(node->right, size);
}
