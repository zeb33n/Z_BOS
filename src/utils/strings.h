typedef enum {
  STR_ERR,
  STR_SUC,
} StrStatus;

int strcmp(const char* str, const char* comp);
int strstartswith(const char* str, const char* comp);
StrStatus str2uint(int* out, const char* str, int base);
