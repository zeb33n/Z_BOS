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

  vga_init();

  sprintln(WELCOMEMSG);

  iprintln(0xb1A2E, 16);
  // iprintln(0, 10);
  sprintln("hello!");

  sprintln("lananas\nbanana");

  sprintln("hi dad");

  // for (int i = 0; i < 2000; i++) {
  //   sprintln("a\nb\nc\nd\n");
  // }

  int whatever = 1 / 0;
  return 0;
}
