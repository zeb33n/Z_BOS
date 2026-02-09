#include "../utils/portio.h"

#define MASTER 0xA0
#define SLAVE 0xB0

static inline void delay_400ns() {
  inb(0x3F6);
  inb(0x3F6);
  inb(0x3F6);
  inb(0x3F6);
}

void identify(int drive) {
  outb(0x1F6, drive);
}
