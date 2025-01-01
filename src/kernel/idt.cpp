#include "keyboard.h"
#include "memory.h"
#include "portio.h"
#include "printing.h"

extern "C" void _idt_load();

extern "C" void _isr_generic();

extern "C" void _irq_keyboard();

extern "C" void _irq_under_40();

extern "C" void _irq_over_39();

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

void idt_install() {
  _idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  _idtp.base = (unsigned long)&idt;

  // Clear out the entire IDT, initializing it to zeros
  memset(&idt, 0, sizeof(struct idt_entry) * 256);

  _idt_load();
}

extern "C" void _fault_handler() {
  sprintln("Exception!");
}

void isr_install() {
  // for (int i = 0; i < 32; i++) {
  //   idt_set_gate(i, (unsigned long)_isr_generic, 0x08, 0x8E);
  // }
  idt_set_gate(0, (unsigned long)_isr_generic, 0x08, 0x8E);
}

// moves the irqs from the default position to start at isr 32
// since by default they overlap with the protected mode isrs
// reserved for exceptions.
void irq_remap() {
  outb(0x20, 0x11);  // Initialize both PICs
  outb(0xA0, 0x11);
  outb(0x21, 0x20);  // Set vector offsets
  outb(0xA1, 0x28);
  outb(0x21, 0x04);  // tell Master PIC theres a slave PIC at IRQ2 (0000 0100)
  outb(0xA1, 0x02);  // tell Slave PIC its cascade identity (0000 0010)
  outb(0x21, 0x01);  // have the PICs use 8086 mode (and not 8080 mode)
  outb(0xA1, 0x01);
  outb(0x21, 0x0);  // set masks
  outb(0xA1, 0x0);
}

extern "C" void _more_than_39() {
  // sprintln("more");
  outb(0xA0, 0x20);  // EOI to pic2
  outb(0x20, 0x20);  // EOI to pic1
}

extern "C" void _less_than_40() {
  // sprintln("less");
  outb(0x20, 0x20);
}

// not working try timer one instead

extern "C" void _keyboard_handler() {
  keyboard_handle();
  // sprintln("finished");
  outb(0x20, 0x20);
  // sprintln("double finished");
}

void irq_install() {
  irq_remap();

  idt_set_gate(32, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(33, (unsigned long)_irq_keyboard, 0x08, 0x8E);
  idt_set_gate(34, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(35, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(36, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(37, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(38, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(39, (unsigned long)_irq_under_40, 0x08, 0x8E);
  idt_set_gate(40, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(41, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(42, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(43, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(44, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(45, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(46, (unsigned long)_irq_over_39, 0x08, 0x8E);
  idt_set_gate(47, (unsigned long)_irq_over_39, 0x08, 0x8E);
  iprintln((unsigned long)_irq_keyboard, 16);
}
