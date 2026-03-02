#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/memory.h"
#include "../utils/types.h"
#include "filesystem.h"

FileTableEntry root{STOP, FOLDER, Folder{0, "root", {}}};

FileTable FT = {1, 10, &root};

void write_file_table() {
  int ft_size = sizeof(FileTableEntry) * FT.count;
  write_28bit(MASTER, 5, (ft_size + 511) / 512, (short*)FT.entrys);
}

void report_status(FileSystemStatus status) {
  switch (status) {
    case FS_ERR_CORRUPT_DISK:
      return sprintln("corrupt disk");
    case FS_SUCCESS:
      return sprintln("ok");
  }
}

// relies on FileTableEntries being 64 byte aligned in disk
// we should parameterize this down the line
// lba gets clobbered to 0 by read_28bt
FileSystemStatus read_file_table() {
  FT.count = 0;
  FT.capacity = 10;
  FT.entrys = (FileTableEntry*)kmalloc(sizeof(FileTableEntry) * FT.capacity);

  FileTableEntry ft_buffer[512 / sizeof(FileTableEntry)];
  for (int lba = 5;; lba++) {
    memset(ft_buffer, 0, sizeof(FileTableEntry) * 8);
    read_28bit(MASTER, lba, 1, (short*)ft_buffer);
    for (int i = 0; i < 4; i++) {
      if (ft_buffer[i].next != CONTINUE && ft_buffer[i].next != STOP) {
        return FS_ERR_CORRUPT_DISK;
      }
      FT.entrys[FT.count] = ft_buffer[i];
      FT.count++;
      if (FT.count > FT.capacity) {
        FT.capacity *= 2;
        FT.entrys = (FileTableEntry*)krealloc(FT.entrys, FT.capacity);
      }

      if (ft_buffer[i].next != CONTINUE) {  // magic byte
        goto read_file_table_loop_esc;
      }
    }
  }
read_file_table_loop_esc:
  iprintln(FT.entrys[0].kind, 10);
  return FS_SUCCESS;
}

void init_file_system() {
  write_file_table();
  read_file_table();
}
