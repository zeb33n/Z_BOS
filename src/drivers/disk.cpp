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

int disk_status_handle(DiskStatus s) {
  switch (s) {
    case DISK_ERR_NO_DRIVE:
      sprintln("NO DRIVE FOUND");
      return 0;
    case DISK_ERR_NOT_ATA:
      sprintln("DISK IS NOT ATA");
      return 0;
    case DISK_ERR_UNRECOGNISED_CMD:
      sprintln("UNRECOGNISED DISK COMMAND");
      return 0;
    case DISK_ERR_SECTOR_OVERFLOW:
      sprintln("SECTOR OVERFLOW");
      return 0;
    case DISK_SUCCESS:
      return 1;
  }
}

void disk_drive_init(int drive) {
  if (!disk_status_handle(identify(drive))) {
    return;
  }
  disk_info.sectors28 = *(int*)(data_buffer + 60);
  disk_info.sectors48 = *(long long*)(data_buffer + 100);
  iprintln(disk_info.sectors28, 10);
}

void read_io_port(short* ptr) {
  *ptr = inw(0x1F0);
}

void write_io_port(short* ptr) {
  outw(0x1F0, *ptr);
}

void send_28bit_command(int drive, int lba, char count, short* ptr, char cmd) {
  if (!disk_status_handle(identify(drive))) {
    return;
  }
  // Wait until not busy
  while (inb(0x1F7) & 0x80) {
  }

  delay_400ns();
  outb(0x1F6, (drive + 0x40) | ((lba >> 24) & 0x0F));
  outb(0x1F2, count);
  outb(0x1F3, (char)lba);
  outb(0x1F4, (char)(lba >> 8));
  outb(0x1F5, (char)(lba >> 16));
  outb(0x1F7, cmd);
  delay_400ns();

  void (*io_func)(short*);
  switch (cmd) {
    case WRITE:
      io_func = &write_io_port;
      break;
    case READ:
      io_func = &read_io_port;
      break;
    default:
      disk_status_handle(DISK_ERR_UNRECOGNISED_CMD);
  }

  for (int i = 0; i < count; i++) {
    // Wait until not busy
    while (inb(0x1F7) & 0x80) {
    }
    // wait until ready
    while (!(inb(0x1F7) & 0x40)) {
    }
    for (int j = 0; j < 256; j++) {
      io_func(&(ptr[j]));
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
