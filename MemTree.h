typedef struct Pair {
  int start;
  int end;
  int size;
  struct Pair *left;
  struct Pair *right;
  int allocated;
  int fragmentation;
  int depth;
  int horizontal;
} Pair;

typedef struct MemTree {
  struct Pair *root;
} MemTree;

void createMemTree(struct Pair *root, int sizeMem);
void destruct(struct Pair *root);

void findBlock(MemTree *tree, struct Pair *node, int size, struct Pair **block, int *success);
void split(MemTree *tree, struct Pair *node);
void delete(MemTree *tree, struct Pair *node);
void merge(MemTree *tree, struct Pair *node);
void deallocate(MemTree *tree, int start);
