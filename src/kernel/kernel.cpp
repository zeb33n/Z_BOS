#include "../drivers/disk.h"
#include "../drivers/keyboard.h"
#include "../drivers/printing.h"
#include "../shell/shell.h"
#include "../utils/memory.h"
#include "../utils/strings.h"
#include "../utils/timer.h"
#include "filesystem.h"
#include "idt.h"

// TODO:
// 0. memory detection -> use multiboot
// 1. kmalloc -> needed for page frame allocation -> first free aligned address
// after the kernel https://wiki.osdev.org/Memory_Allocation
// 2. paging https://wiki.osdev.org/Paging,
// https://wiki.osdev.org/Setting_Up_Paging
//   - identity paging
//   - uses the ps register
// 3. processes for malloc https://wiki.osdev.org/Writing_a_memory_manager

extern "C" int _start() {
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

  // test disk driver
  // TODO tests to their own file
  sprint("test disk... ");
  char writestr[256] = "bananas";
  write_28bit(MASTER, 1, 1, (short*)writestr);
  char readstr[512];
  read_28bit(MASTER, 1, 2, (short*)readstr);
  if (strcmp("bananas", readstr)) {
    sprintln("[ok]");
  } else {
    sprintln("[error]");
  }

  // Need to debug why this doesnt work
  sprint("test kmalloc... ");
  void* a = kmalloc(0x40);
  kmalloc(0x40);
  kfree(a);
  long b = (long)kmalloc(0x81);
  long c = (long)kmalloc(64);
  long d = (long)kmalloc(0x70);
  if (b == HEAP_BASE + SLAB_SIZE * 2 && c == HEAP_BASE &&
      d == HEAP_BASE + SLAB_SIZE * 5) {
    sprintln("[ok]");
  } else {
    sprintln("[error]");
  }

  shell_init();
  for (;;) {
  }
  return 0;
}
