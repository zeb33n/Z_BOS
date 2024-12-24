#include "idt.h"
#include "printing.h"

// TODO:
// 0. sort out file structure
// 1. interrupts IDT
// 2. kmalloc -> needed for page frame allocation -> first free aligned address
// after the kernel https://wiki.osdev.org/Memory_Allocation
// 3. paging https://wiki.osdev.org/Paging,
// https://wiki.osdev.org/Setting_Up_Paging
//   - identity paging
//   - uses the ps register
// 4. processes for malloc https://wiki.osdev.org/Writing_a_memory_manager

extern "C" int main() {
  // Notice how these functions are never called
  // idt_install();
  // isr_install();
  char string[] = "Welcome to zeb OS!";
  write_base_int((unsigned long)string, 16, 0xA, 0x1, 32, 10);
  write_string(string, 0x0, 0xC, 10, 12);

  write_char('B', 0xA, 0x1, 30, 2);
  write_char('Q', 0xA, 0x1, 32, 4);
  write_char('D', 0xA, 0x1, 30, 4);
  write_base_int(0xb1A2E, 16, 0xA, 0x1, 32, 3);
  return 0;
}
