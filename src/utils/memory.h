#define HEAP_BASE 0x10000
#define HEAP_END 0x20040
#define SLAB_SIZE 64

typedef struct Slab Slab;

typedef struct Slab {
  Slab* next;
} Slab;

void memcopy(void* dest, void* data, int count);
void* memset(void* dest, unsigned char val, int count);
void* kmalloc(int size);
void kheap_init();
