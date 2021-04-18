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
    return node;
  }
  else if (size == node->size && !node->allocated)
  	return node;
  else if (size < node->size)
  	findBlock(node->left, size);
  else
  	findBlock(node->right, size);
}

void split(MemTree *tree, struct Pair *node{

    int newSize = node->size / 2;
    int leftStart = node->start;
    int rightStart = leftStart + newSize;
    int leftEnd = leftStart + newSize - 1;
    int rightEnd = rightStart + newSize - 1;

    node->left = (struct Pair *) malloc(sizeof(struct Pair));
    node->left->size = newSize;
    node->left->start = leftStart;
    node->left->end = leftEnd;

    node->right = (struct Pair *) malloc(sizeof(struct Pair));
    node->right->size = newSize;
    node->right->start = rightStart;
    node->right->end = rightEnd;
}

void merge(MemTree *tree, struct Pair *node) {
  if (node->left != NULL) {
    merge(tree, node->left);
    if (node->left->allocated == 0 && node->right->allocated == 0 && (node->left->left == NULL || node->right->left == NULL)) {
      free(node->left);
      free(node->right);
      node->left = NULL;
      node->right = NULL;
    }
    if (node->right != NULL)
      merge(tree, node->right);
	}
}
