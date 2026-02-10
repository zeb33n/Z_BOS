#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/portio.h"

static short data_buffer[256];

static DiskInfo disk_info;

static inline void delay_400ns() {
  inb(0x3F6);
  inb(0x3F6);
  inb(0x3F6);
  inb(0x3F6);
}

DiskStatus identify(int drive) {
  // select the drive (slave or master)
  outb(0x1F6, drive);
  delay_400ns();

  // set ports to 0
  for (int i = 0x1F2; i < 0x1F6; i++) {
    outb(i, 0);
  }

  // send IDENTIFY command
  outb(0x1F7, 0xEC);
  // read output
  if (inb(0x1F7) == 0) {
    return DISK_ERR_NO_DRIVE;
  }

  // Wait until not busy
  while (inb(0x1F7) & 0x80) {
  }

  // check LBAmid and LBAhi
  if (inb(0x1F4) != 0 || inb(0x1F5) != 0) {
    return DISK_ERR_NOT_ATA;
  }

  // wait until ready
  while (!(inb(0x1F7) & 0x40)) {
  }

  // read the data
  for (int i = 0; i < 256; i++) {
    data_buffer[i] = inw(0x1F0);
  }
  return DISK_SUCCESS;
}

int check_status(DiskStatus s) {
  switch (s) {
    case DISK_ERR_NO_DRIVE:
      sprintln("NO DRIVE FOUND");
      return 0;
    case DISK_ERR_NOT_ATA:
      sprintln("DISK IS NOT ATA");
      return 0;
    default:
      return 1;
  }
}

void disk_drive_init(int drive) {
  if (!check_status(identify(drive))) {
    return;
  }
  disk_info.sectors28 = *(int*)(data_buffer + 60);
  disk_info.sectors48 = *(long long*)(data_buffer + 100);
}

void send_28bit_command(int drive,
                        int lba,
                        char sectorcount,
                        short* ptr,
                        char cmd) {
  if (!check_status(identify(drive))) {
    return;
  }
  // Wait until not busy
  while (inb(0x1F7) & 0x80) {
  }

  delay_400ns();
  outb(0x1F6, (drive + 0x40) | ((lba >> 24) & 0x0F));
  outb(0x1F2, sectorcount);
  outb(0x1F3, (char)lba);
  outb(0x1F4, (char)(lba >> 8));
  outb(0x1F5, (char)(lba >> 16));
  outb(0x1F7, cmd);
  delay_400ns();

  for (int i = 0; i < sectorcount; i++) {
    // Wait until not busy
    while (inb(0x1F7) & 0x80) {
    }
    // wait until ready
    while (!(inb(0x1F7) & 0x40)) {
    }
    for (int j = 0; j < 256; j++) {
      // use a function pointer here maybe
      if (cmd == WRITE) {
        outw(0x1F0, ptr[j]);
      }
      if (cmd == READ) {
        ptr[j] = inw(0x1F0);
      }
    }
    ptr += 256;
  }
}

void read_28bit(int drive, int lba, char sectorcount, short* ptr) {
  send_28bit_command(drive, lba, sectorcount, ptr, READ);
}

void write_28bit(int drive, int lba, char sectorcount, short* ptr) {
  send_28bit_command(drive, lba, sectorcount, ptr, WRITE);
}
