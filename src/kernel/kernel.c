#include "../drivers/disk.h"
#include "../drivers/keyboard.h"
#include "../drivers/printing.h"
#include "../shell/shell.h"
#include "../utils/memory.h"
#include "../utils/timer.h"
#include "filesystem.h"
#include "idt.h"

#ifdef TEST
#include "../test/tests.h"
#endif

// TODO:
// 0. memory detection -> use multiboot
// 1. kmalloc -> needed for page frame allocation -> first free aligned address
// after the kernel https://wiki.osdev.org/Memory_Allocation
// 2. paging https://wiki.osdev.org/Paging,
// https://wiki.osdev.org/Setting_Up_Paging
//   - identity paging
//   - uses the ps register
// 3. processes for malloc https://wiki.osdev.org/Writing_a_memory_manager

typedef struct {
  int count;
  int capacity;
  int* values;
} DynInts;

extern int _start() {
  init_pit(1931);  // one tick every ms
  idt_install();
  isr_install();
  irq_install();

  asm volatile("sti");

  vga_init();
  disk_drive_init(MASTER);
  kheap_init();
  keyboard_default();

  sprintln(WELCOMEMSG);

#ifdef TEST
  run_tests();
#endif

  init_file_system();

  shell_init();
  for (;;) {
  }
  return 0;
}
