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
  // easy exit if we need less than one slab of mem
  Slab* out_ptr = free_list;
  // if (size <= SLAB_SIZE) {
  //   free_list = free_list->next;
  //   return out_ptr;
  // }

  // find n contiguous slabs
  int n_contig_slabs = (size + SLAB_SIZE - 1) / SLAB_SIZE;
  Slab* last = (Slab*)NULL;
  Slab* slab = free_list;

  while (slab != NULL) {
    int count = n_contig_slabs - 1;
    // Slab* prev;
    while ((char*)slab->next - SLAB_SIZE == (char*)slab && count) {
      // prev = slab;
      slab = slab->next;
      count--;
    }

    if (!last && !count) {
      free_list = slab->next;
      return (void*)out_ptr;
    } else if (!count) {
      // sprint("prev: ");
      // iprintln((long)prev, 16);
      last->next = slab->next;
      return (void*)out_ptr;
    }

    out_ptr = slab->next;
    last = slab;
    slab = slab->next;
  }

  return NULL;
}

void kfree(void* ptr) {
  Slab* slab = (Slab*)ptr;
  slab->next = free_list;
  free_list = slab;
}
