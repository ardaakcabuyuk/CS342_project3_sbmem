typedef struct Pair {
  int start;
  int end;
  int size;
  struct Pair *left = NULL;
  struct Pair *right = NULL;
  int allocated = 0;
  int fragmentation;
} Pair;

typedef struct {
  struct Pair *root;
} MemTree;

MemTree *createMemTree(int sizeMem);
void destruct(Pair *root);

struct Pair *findBlock(MemTree *tree, struct Pair *node, int size);
void split(MemTree *tree, struct Pair *node);
void delete(MemTree *tree, struct Pair *node);
