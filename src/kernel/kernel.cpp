#include "idt.h"
#include "printing.h"

// TODO:
// 0. TTY
// 1. interrupts IDT
// 2. kmalloc -> needed for page frame allocation -> first free aligned address
// after the kernel https://wiki.osdev.org/Memory_Allocation
// 3. paging https://wiki.osdev.org/Paging,
// https://wiki.osdev.org/Setting_Up_Paging
//   - identity paging
//   - uses the ps register
// 4. processes for malloc https://wiki.osdev.org/Writing_a_memory_manager

extern "C" int _start() {
  idt_install();
  isr_install();
  irq_install();

  asm volatile("sti");

  vga_init();

  sprintln(WELCOMEMSG);

  iprintln(0xB1A2E, 16);
  // iprintln(0, 10);
  sprintln("hello!");

  sprintln("lananas\nbanana");

  // int whatever = 1 / 0;
  sprintln("hi dad");

  for (;;) {
  }
  return 0;
}
