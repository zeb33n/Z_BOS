#include "portio.h"

volatile int countdown = 0;

// default frequency is 1.193182MHz
// divide by count to set interval
void init_pit(unsigned count) {
  outb(0x43,
       0b00110100);  // set the pit channel 0, low/high mode, rate generator
  asm volatile("cli");
  outb(0x40, count & 0xFF);
  outb(0x40, (count & 0xFF00) >> 8);
}

void timer_handle() {
  if (countdown == 0) {
    return;
  }
  countdown--;
}

void sleep(int millis) {
  countdown = millis;
  while (countdown > 0) {
  }
}
