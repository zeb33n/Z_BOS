#include "maths.h"

int strstartswith(char* str, const char* comp) {
  int out = 1;
  for (int i = 0;; i++) {
    if (comp[i] == '\0') {
      return 1;
    }
    if (str[i] == '\0') {
      return 0;
    }
    if (str[i] != comp[i]) {
      return 0;
    }
  }
}

int strlen(const char* str) {
  int i = 0;
  do {
    ;
  } while (str[i++] != '\0');
  return i - 1;
}

int str2uint(const char* str, int base) {
  int out = 0;
  int j = 0;
  for (int i = strlen(str) - 1; i >= 0; i--) {
    if (str[i] > 0x3A) {
      return -1;
    }
    out += (str[i] - 0x30) * pow(base, j);
    j++;
  }
  return out;
}
