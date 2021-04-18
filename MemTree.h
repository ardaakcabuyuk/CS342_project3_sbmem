typedef struct Pair {
  int start;
  int end;
  int size;
  struct Pair *left;
  struct Pair *right;
  int allocated;
  int fragmentation;
} Pair;

typedef struct {
  struct Pair *root;
} MemTree;

MemTree *createMemTree(int sizeMem);
void destruct(struct Pair *root);

void findBlock(MemTree *tree, struct Pair *node, int size, struct Pair **block, int *success);
void split(MemTree *tree, struct Pair *node);
void delete(MemTree *tree, struct Pair *node);
void merge(MemTree *tree, struct Pair *node);
