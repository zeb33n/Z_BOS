
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

#define dyn_init_default(arr)                                 \
  do {                                                        \
    arr.count = 0;                                            \
    arr.capacity = SLAB_SIZE / sizeof(*arr.values);           \
    arr.values = kmalloc(arr.capacity * sizeof(*arr.values)); \
  } while (0)

#define dyn_init_size(arr, size)                              \
  do {                                                        \
    arr.count = 0;                                            \
    arr.capacity = size;                                      \
    arr.values = kmalloc(arr.capacity * sizeof(*arr.values)); \
  } while (0)

#define dyn_rm(arr, index)                        \
  do {                                            \
    for (int i = index; i < arr.count - 1; i++) { \
      arr.values[i] = arr.values[i + 1];          \
    }                                             \
    arr.count--;                                  \
  } while (0)

#define _dyn_init_get(_1, _2, NAME, ...) NAME
#define dyn_init(...) \
  _dyn_init_get(__VA_ARGS__, dyn_init_size, dyn_init_default)(__VA_ARGS__)

#endif
