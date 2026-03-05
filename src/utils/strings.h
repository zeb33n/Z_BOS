#ifndef STRINGS
#define STRINGS

typedef enum {
  STR_ERR,
  STR_SUC,
} StrStatus;

typedef struct {
  int count;
  int capacity;
  char* values;
} DynStr;

int strcmp(const char* str, const char* comp);
int strstartswith(const char* str, const char* comp);
int strlen(const char* str);
StrStatus strcpy(const char* from, char* to, int size);
StrStatus str2uint(int* out, const char* str, int base);

#endif
