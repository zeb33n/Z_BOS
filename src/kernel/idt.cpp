#include "memory.h"
#include "printing.h"

extern "C" void _idt_load();

extern "C" void _isr_generic();

struct idt_entry {
  unsigned short base_lo;
  unsigned short sel;
  unsigned char always0;
  unsigned char flags;
  unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr _idtp;

extern "C" void _fault_handler() {
  sprintln("Exception!");
}

void idt_set_gate(unsigned char num,
                  unsigned long base,
                  unsigned short sel,
                  unsigned char flags) {
  idt[num].base_hi = (short)(base >> 16) & 0xffff;
  idt[num].base_lo = (short)(base & 0xffff);
  idt[num].always0 = 0;
  idt[num].sel = sel;
  idt[num].flags = flags;
}

// now we just need to set interrupt request functions using assemblt
// first 32 are a good place to start

void idt_install() {
  _idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  _idtp.base = (unsigned long)&idt;

  // Clear out the entire IDT, initializing it to zeros
  memset(&idt, 0, sizeof(struct idt_entry) * 256);

  _idt_load();
}

// theres nos issues with the code. just where it is. its overwriting itself?!
void isr_install() {
  idt_set_gate(0, (unsigned long)_isr_generic, 0x08, 0x8E);
}
