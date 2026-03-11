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

// TODO make sure you dont overwrite the stack
// TODO error handling
// slab allocator
void* kmalloc(int size) {
  Slab* prev = free_list;
  Slab* last = (Slab*)NULL;
  Slab* slab = free_list;
  // extra bytes for int that measures size of pointer
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

    // special bytes behind pointer contain
    // number of slabs in allocation
    int* count_int = (int*)prev;
    *count_int = n_contig_slabs;
    return (void*)(count_int + 1);
  }

  return NULL;
}

// TODO what if n_slabs is less than 1
// TODO somethink to check pointer is aligned as expected
void kfree(void* ptr) {
  int* base = (int*)ptr - 1;
  int n_slabs = *base;
  long slab_addr = (long)base + (n_slabs - 1) * SLAB_SIZE;
  long prev_addr = (long)free_list;
  for (;;) {
    Slab* slab = (Slab*)slab_addr;
    slab->next = (Slab*)prev_addr;
    prev_addr = slab_addr;
    n_slabs--;
    if (n_slabs <= 0) {
      break;
    }
    slab_addr -= SLAB_SIZE;
  }
  free_list = (Slab*)slab_addr;
}

void* krealloc(void* ptr, int size) {
  void* new_ptr = kmalloc(size);
  int n_slabs = *((int*)ptr - 1);
  int count = SLAB_SIZE * n_slabs;
  memcopy(new_ptr, ptr, count);
  kfree(ptr);
  return new_ptr;
}
