#include "../drivers/disk.h"
#include "../utils/types.h"
#include "filesystem.h"

// TODO: upgrade this with kmalloc

FileTableEntry root{0, FOLDER, Folder{0, "root", {}}};

FileTable FT = {1, &root};

void write_file_table() {
  int ft_size = sizeof(FileTableEntry) * FT.count;
  write_28bit(MASTER, 0, (ft_size + 511) / 512, (short*)FT.entrys);
}

void read_file_table() {}
