#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/memory.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

FileTableEntry root{STOP, FOLDER, Folder{0, "root", {}}};

FileTable FT = {1, 10, &root};

Folder current_folder = root.data.folder;

void write_file_table() {
  int ft_size = sizeof(FileTableEntry) * FT.count;
  write_28bit(MASTER, 1, (ft_size + 511) / 512, (short*)FT.entrys);
}

void report_status(FileSystemStatus status) {
  switch (status) {
    case FS_ERR_CORRUPT_DISK:
      return sprintln("corrupt disk");
    case FS_SUCCESS:
      return sprintln("ok");
    case FS_ERR_NAME_LENGTH:
      return sprintln("name is too long");
    case FS_ERR_FOLDER_CAP:
      return sprintln("folder at max capacity");
  }
}

// relies on FileTableEntries being 64 byte aligned in disk
FileSystemStatus read_file_table() {
  FT.count = 0;
  FT.capacity = 10;
  FT.entrys = (FileTableEntry*)kmalloc(sizeof(FileTableEntry) * FT.capacity);

  FileTableEntry ft_buffer[512 / sizeof(FileTableEntry)];
  for (int lba = 1;; lba++) {
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
  return FS_SUCCESS;
}

// TODO print in different colours
void list_current_folder() {
  for (int i = 0; i < current_folder.numfiles; i++) {
    FileTableEntry entry = *current_folder.entrys[i];
    if (entry.kind == FOLDER) {
      sprintln(entry.data.folder.name);
    } else if (entry.kind == FILE) {
      sprintln(entry.data.file.name);
    }
  }
}

FileSystemStatus create_file_in_current_folder(const char* name) {
  File f;
  f.size = 0;
  f.start_byte = 0;
  f.start_sector = 0;
  if (strcpy(name, f.name, strlen(name)) != STR_SUC) {
    return FS_ERR_NAME_LENGTH;
  }
  FileTableEntry entry;
  entry.data.file = f;
  entry.kind = FILE;
  entry.next = STOP;

  FT.entrys[FT.count].next = CONTINUE;
  FT.count++;
  if (FT.count > FT.capacity) {
    FT.capacity *= 2;
    FT.entrys = (FileTableEntry*)krealloc(FT.entrys, FT.capacity);
  }
  FT.entrys[FT.count] = entry;
  write_file_table();

  if (current_folder.numfiles++ > 10) {
    current_folder.numfiles -= 1;
    return FS_ERR_FOLDER_CAP;
  }
  current_folder.entrys[current_folder.numfiles - 1] = &(FT.entrys[FT.count]);
  return FS_SUCCESS;
}

void init_file_system() {
  write_file_table();
  report_status(read_file_table());
  report_status(create_file_in_current_folder("zebbbb"));
  list_current_folder();
}
