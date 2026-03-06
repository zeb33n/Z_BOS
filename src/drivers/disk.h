#ifndef DISK
#define DISK

#define MASTER 0xA0
#define SLAVE 0xB0

#define READ 0x20
#define WRITE 0x30

typedef enum {
  DISK_ERR_NO_DRIVE,
  DISK_ERR_NOT_ATA,
  DISK_ERR_SECTOR_OVERFLOW,
  DISK_ERR_UNRECOGNISED_CMD,
  DISK_SUCCESS,
} DiskStatus;

typedef struct {
  int sectors28;
  int sectors48;
} DiskInfo;

void disk_drive_init(int drive);
void read_28bit(int drive, int lba, char sectorcount, short* ptr);
void write_28bit(int drive, int lba, char sectorcount, short* ptr);

extern DiskInfo disk_info;

#endif
