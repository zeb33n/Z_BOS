#ifdef TEST

#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/memory.h"
#include "../utils/strings.h"

// TODO fix broken disk driver
void test_disk_driver() {
  sprint("test disk... ");
  char err = 0;

  char writestr[256] = "bananas";
  write_28bit(MASTER, 1, 1, (short*)writestr);
  char readstr[512];
  read_28bit(MASTER, 1, 1, (short*)readstr);
  if (!strcmp("bananas", readstr)) {
    sprintln("1");
    err = 1;
  }

  char writestr512[512];
  memset(writestr512, 'a', 511);
  writestr512[511] = '\0';
  write_28bit(MASTER, 4, 2, (short*)writestr512);
  char readstr512[512];
  read_28bit(MASTER, 4, 2, (short*)readstr512);
  char target[512];
  memset(target, 'a', 511);
  target[511] = '\0';
  if (!strcmp(target, readstr512)) {
    err = 1;
  }

  if (!err) {
    sprintln("[ok]");
  } else {
    sprintln("[error]");
  }
}

void test_memory() {
  sprint("test kmalloc... ");
  void* a = kmalloc(SLAB_SIZE - sizeof(int));
  void* x = kmalloc(SLAB_SIZE * 2 - sizeof(int));
  kfree(a);
  long b = (long)kmalloc(SLAB_SIZE * 2 + 1 - sizeof(int));
  long c = (long)kmalloc(SLAB_SIZE - sizeof(int));
  long d = (long)kmalloc(SLAB_SIZE - sizeof(int) + 0x20);
  char err = 0;
  if (b != HEAP_BASE + SLAB_SIZE * 3 + sizeof(int)) {
    sprint("b: ");
    iprintln(b, 16);
    err = 1;
  }
  if (c != HEAP_BASE + sizeof(int)) {
    sprint("c: ");
    iprintln(c, 16);
    err = 1;
  }
  if (d != HEAP_BASE + SLAB_SIZE * 6 + sizeof(int)) {
    sprint("d: ");
    iprintln(d, 16);
    err = 1;
  }
  kfree((void*)b);
  long e = (long)kmalloc(SLAB_SIZE * 3 - sizeof(int));
  if (e != HEAP_BASE + SLAB_SIZE * 3 + sizeof(int)) {
    sprint("e: ");
    iprintln(e, 16);
    iprintln(HEAP_BASE + SLAB_SIZE * 3 + sizeof(int), 16);
    err = 1;
  }
  if (err) {
    sprintln("[error]");
  } else {
    sprintln("[ok]");
    kfree((void*)c);
    kfree((void*)d);
    kfree((void*)e);
    kfree((void*)x);
  }
}

void run_tests() {
  test_disk_driver();
  test_memory();
}

#endif
