#include "memory.h"
#include "printing.h"

char BUFFER[256];

void shell_init() {
  memset(BUFFER, '\0', sizeof(char) * 256);
  cprint('>');
}
