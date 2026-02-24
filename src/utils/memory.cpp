#include "../drivers/printing.h"
#include "memory.h"
#include "types.h"

Slab* free_list;

void* memset(void* dest, unsigned char val, int count) {
  unsigned char* destC = (unsigned char*)dest;
  for (int i = 0; i < count; i++) {
    destC[i] = val;
  }
  return dest;
}

void memcopy(void* dest, void* data, int count) {
  unsigned char* destC = (unsigned char*)dest;
  unsigned char* dataC = (unsigned char*)data;
  for (int i = 0; i < count; i++) {
    destC[i] = dataC[i];
  }
}

// make a linked list of slabs point to each other in the free memory
void kheap_init() {
  Slab* prev = (Slab*)(HEAP_END - SLAB_SIZE);
  prev->next = (Slab*)NULL;
  for (long addr = HEAP_END - SLAB_SIZE; addr >= HEAP_BASE; addr -= SLAB_SIZE) {
    Slab* slab = (Slab*)addr;
    slab->next = prev;
    prev = slab;
  }
  free_list = prev;
}

// slab allocator
// TODO make sure you dont overwrite the stack
void* kmalloc(int size) {
  Slab* prev = free_list;
  Slab* last = (Slab*)NULL;
  Slab* slab = free_list;
  // extra 4 bytes for int that measures size of pointer
  int n_contig_slabs = (size + sizeof(int) + SLAB_SIZE - 1) / SLAB_SIZE;

  // find n contiguous slabs
  while (slab != NULL) {
    int count = n_contig_slabs - 1;
    while ((char*)slab->next - SLAB_SIZE == (char*)slab && count) {
      slab = slab->next;
      count--;
    }

    if (count) {
      prev = slab->next;
      last = slab;
      slab = slab->next;
      continue;
    }

    if (!last) {
      free_list = slab->next;
    } else {
      last->next = slab->next;
    }

    // special 4 bytes behind pointer contain
    // number of slabs in allocation
    int* count_int = (int*)prev;
    *count_int = n_contig_slabs;
    return (void*)(count_int + 1);
  }

  return NULL;
}

void kfree(void* ptr) {
  int* base = (int*)ptr - 1;
  int n_slabs = *base;
  Slab* slab = (Slab*)base + n_slabs - 1;
  Slab* prev = free_list;
  while (n_slabs > 0) {
    slab->next = prev;
    prev = slab;
    slab -= 1;
    n_slabs--;
  }
  free_list = slab + 1;
}
