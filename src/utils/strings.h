typedef enum {
  STR_ERR,
  STR_SUC,
} StrStatus;

int strcmp(const char* str, const char* comp);
int strstartswith(const char* str, const char* comp);
int strlen(const char* str);
StrStatus strcpy(const char* from, char* to, int size);
StrStatus str2uint(int* out, const char* str, int base);
