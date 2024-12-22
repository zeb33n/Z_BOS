// #include <system.h>

void* memset(void* dest, unsigned char val, int count) {
  unsigned char* destC = (unsigned char*)dest;
  int i;
  for (i = 0; i < count; i++)
    destC[i] = val;
  return dest;
}

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

extern void _idt_load();

void idt_set_gate(unsigned char num,
                  unsigned int base,
                  unsigned short sel,
                  unsigned char flags) {
  idt[num].base_hi = (short)(base >> 16);
  idt[num].base_lo = (short)(base & 0xffff);
  idt[num].always0 = 0x00;
  idt[num].sel = sel;
  idt[num].flags = flags;
}

void idt_install() {
  _idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  _idtp.base = (unsigned int)&idt;

  // Clear out the entire IDT, initializing it to zeros
  memset(&idt, 0, sizeof(struct idt_entry) * 256);

  _idt_load();
}
