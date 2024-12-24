#include "printing.h"

extern "C" void _idt_load();

extern "C" void _idt_load();
extern "C" void _isr0();
extern "C" void _isr1();
extern "C" void _isr2();
extern "C" void _isr3();
extern "C" void _isr4();
extern "C" void _isr5();
extern "C" void _isr6();
extern "C" void _isr7();
extern "C" void _isr8();
extern "C" void _isr9();
extern "C" void _isr10();
extern "C" void _isr11();
extern "C" void _isr12();
extern "C" void _isr13();
extern "C" void _isr14();
extern "C" void _isr15();
extern "C" void _isr16();
extern "C" void _isr17();
extern "C" void _isr18();
extern "C" void _isr19();
extern "C" void _isr20();
extern "C" void _isr21();
extern "C" void _isr22();
extern "C" void _isr23();
extern "C" void _isr24();
extern "C" void _isr25();
extern "C" void _isr26();
extern "C" void _isr27();
extern "C" void _isr28();
extern "C" void _isr29();
extern "C" void _isr30();
extern "C" void _isr31();
extern "C" void _syscall();

typedef struct regs {
  unsigned int gs, fs, es, ds;                          // pushed the segs last
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by 'pusha'
  unsigned int int_no, err_code;  // our 'push byte #' and ecodes do this
  unsigned int eip, cs, eflags, useresp,
      ss;  // pushed by the processor automatically
} __attribute__((packed)) regs;
// todo move to memory file
void* memset(void* dest, unsigned char val, int count) {
  unsigned char* destC = (unsigned char*)dest;
  int i;
  for (i = 0; i < count; i++)
    destC[i] = val;
  return dest;
}

extern "C" void _fault_handler(struct regs* r) {
  if (r->int_no == -0x80) {  // heh, unsigned, lazy
    write_char('e', 0xE, 0x1, 20, 20);
  }
  if (r->int_no < 32) {
    write_char('e', 0xE, 0x1, 20, 20);
  }
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

  // I tried printing the memory address of the isrs and coulndt think of any
  // obvious overlaps
  write_base_int((unsigned long)&idt, 16, 0xF, 0x1, 1, 1);
  write_base_int((unsigned long)_isr0, 16, 0xF, 0x1, 1, 2);

  memset(&idt, 0, sizeof(struct idt_entry) * 256);

  _idt_load();
}

// theres nos issues with the code. just where it is. its overwriting itself?!
void isr_install() {
  // idt_set_gate(0, (unsigned long)_isr0, 0x08, 0x8E);
  // idt_set_gate(1, (unsigned long)_isr1, 0x08, 0x8E);
  // idt_set_gate(2, (unsigned long)_isr2, 0x08, 0x8E);
  // idt_set_gate(3, (unsigned long)_isr3, 0x08, 0x8E);
  // idt_set_gate(4, (unsigned long)_isr4, 0x08, 0x8E);
  // idt_set_gate(5, (unsigned long)_isr5, 0x08, 0x8E);
  // idt_set_gate(6, (unsigned long)_isr6, 0x08, 0x8E);
  // idt_set_gate(7, (unsigned long)_isr7, 0x08, 0x8E);
  // idt_set_gate(8, (unsigned long)_isr8, 0x08, 0x8E);
  // idt_set_gate(9, (unsigned long)_isr9, 0x08, 0x8E);
  // idt_set_gate(10, (unsigned long)_isr10, 0x08, 0x8E);
  // idt_set_gate(11, (unsigned long)_isr11, 0x08, 0x8E);
  // idt_set_gate(12, (unsigned long)_isr12, 0x08, 0x8E);
  // idt_set_gate(13, (unsigned long)_isr13, 0x08, 0x8E);
  // idt_set_gate(14, (unsigned long)_isr14, 0x08, 0x8E);
  // idt_set_gate(15, (unsigned long)_isr15, 0x08, 0x8E);
  // idt_set_gate(16, (unsigned long)_isr16, 0x08, 0x8E);
  // idt_set_gate(17, (unsigned long)_isr17, 0x08, 0x8E);
  // idt_set_gate(18, (unsigned long)_isr18, 0x08, 0x8E);
  // idt_set_gate(19, (unsigned long)_isr19, 0x08, 0x8E);
  // idt_set_gate(20, (unsigned long)_isr20, 0x08, 0x8E);
  // idt_set_gate(21, (unsigned long)_isr21, 0x08, 0x8E);
  // idt_set_gate(22, (unsigned long)_isr22, 0x08, 0x8E);
  // idt_set_gate(23, (unsigned long)_isr23, 0x08, 0x8E);
  // idt_set_gate(24, (unsigned long)_isr24, 0x08, 0x8E);
  // idt_set_gate(25, (unsigned long)_isr25, 0x08, 0x8E);
  // idt_set_gate(26, (unsigned long)_isr26, 0x08, 0x8E);
  // idt_set_gate(27, (unsigned long)_isr27, 0x08, 0x8E);
  // idt_set_gate(28, (unsigned long)_isr28, 0x08, 0x8E);
  // idt_set_gate(29, (unsigned long long)_isr29, 0x08, 0x8E);
  // idt_set_gate(30, (unsigned long)_isr30, 0x08, 0x8E);
  // idt_set_gate(31, (unsigned long)_isr31, 0x08, 0x8E);
  // idt_set_gate(0x80, (unsigned long long)_syscall, 0x08, 0x8E);
}
