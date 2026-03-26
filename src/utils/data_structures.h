
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

#define dyn_init_N(arr, N)                                    \
  do {                                                        \
    arr.count = 0;                                            \
    arr.capacity = N;                                         \
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
  _dyn_init_get(__VA_ARGS__, dyn_init_N, dyn_init_default)(__VA_ARGS__)

#define dyn_clear(arr) \
  do {                 \
    kfree(arr.values); \
    dyn_init(arr);     \
  } while (0)

#define dyn_copy_from(arr, N, buff)                     \
  do {                                                  \
    arr.count = N;                                      \
    memcopy(arr.values, buff, N * sizeof(*arr.values)); \
  } while (0)

#endif
