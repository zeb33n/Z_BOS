
#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES

#include "memory.h"

// TODO write kpanic
#define dyn_append(arr, v)                                                   \
  do {                                                                       \
    if (arr.count >= arr.capacity) {                                         \
      arr.capacity *= 2;                                                     \
      arr.values = krealloc(arr.values, sizeof(*arr.values) * arr.capacity); \
    }                                                                        \
    arr.values[arr.count++] = v;                                             \
  } while (0)

#define dyn_init(arr)                               \
  do {                                              \
    arr.count = 0;                                  \
    arr.capacity = SLAB_SIZE / sizeof(*arr.values); \
    arr.values = kmalloc(arr.capacity);             \
  } while (0)

#endif
