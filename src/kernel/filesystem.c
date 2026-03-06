#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/data_structures.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

FreeDiskReigon FDR;

Folder current_folder;

void report_status(FileSystemStatus status) {
  switch (status) {
    case FS_ERR_CORRUPT_DISK:
      return sprintln("corrupt disk");
    case FS_ERR_DISK_FULL:
      return sprintln("Disk is full");
    case FS_ERR_NO_NAME:
      return sprintln("corrupt disk");
    case FS_SUCCESS:
      return;
  }
}

void folder_create(Folder* f, const char* name, int lba, int parent_lba) {
  f->lba = lba;
  f->parent_lba = parent_lba;
  dyn_init(f->folders);
  dyn_init(f->files);
  dyn_init(f->name);
  for (int i = 0; i <= strlen(name); i++) {
    dyn_append(f->name, name[i]);
  }
}

FileSystemStatus folder_to_disk(Folder f) {
  int lba_info = f.lba;
  int lba_folders = lba_info + 1;
  int folder_sectors = (sizeof(int) * f.folders.count + 511) / 512;
  int lba_files = lba_folders + folder_sectors;
  int file_sectors = (sizeof(int) * f.files.count + 511) / 512;
  int lba_name = lba_files + file_sectors;
  int name_sectors = (f.name.count + 511) / 512;
  if (lba_name + name_sectors > disk_info.sectors28) {
    return FS_ERR_DISK_FULL;
  }

  FolderDiskRep f_diskrep = {f.lba, f.parent_lba, name_sectors, folder_sectors,
                             file_sectors};
  FolderDiskRepUnion f_union;
  f_union.folder_disk_rep = f_diskrep;
  write_28bit(MASTER, lba_info, 1, f_union.arr);

  if (folder_sectors) {
    int folders_buff[(folder_sectors * 512) / sizeof(int)];
    memcopy(folders_buff, f.folders.values, f.folders.count * sizeof(int));
    write_28bit(MASTER, lba_folders, folder_sectors, (short*)folders_buff);
  }

  if (file_sectors) {
    int files_buff[(file_sectors * 512) / sizeof(int)];
    memcopy(files_buff, f.files.values, f.files.count * sizeof(int));
    write_28bit(MASTER, lba_files, file_sectors, (short*)files_buff);
  }

  if (name_sectors) {
    char name_buff[name_sectors * 512];
    memcopy(name_buff, f.name.values, f.name.count);
    write_28bit(MASTER, lba_name, name_sectors, (short*)name_buff);
  }

  FDR.lba = lba_name + name_sectors;

  return FS_SUCCESS;
}

FileSystemStatus folder_from_disk(int lba, Folder* f) {
  FolderDiskRepUnion f_union;
  read_28bit(MASTER, lba, 1, f_union.arr);

  FolderDiskRep f_diskrep = f_union.folder_disk_rep;

  int lba_folders = f_diskrep.lba + 1;
  int folder_sectors = (sizeof(int) * f_diskrep.folders_size + 511) / 512;
  int lba_files = lba_folders + folder_sectors;
  int file_sectors = (sizeof(int) * f_diskrep.files_size + 511) / 512;
  int lba_name = lba_files + file_sectors;
  int name_sectors = (f_diskrep.name_size + 511) / 512;

  char name_buff[name_sectors * 512];
  if (name_sectors) {
    read_28bit(MASTER, lba_name, name_sectors, (short*)name_buff);
    folder_create(f, name_buff, f_diskrep.lba, f_diskrep.parent_lba);
  } else {
    return FS_ERR_NO_NAME;
  }

  int folders_buff[(folder_sectors * 512) / sizeof(int)];
  if (folder_sectors) {
    read_28bit(MASTER, lba_folders, folder_sectors, (short*)folders_buff);
    for (int i = 0; i < f_diskrep.folders_size; i++) {
      dyn_append(f->folders, folders_buff[i]);
    }
  }

  int files_buff[(file_sectors * 512) / sizeof(int)];
  if (file_sectors) {
    read_28bit(MASTER, lba_files, file_sectors, (short*)files_buff);
    for (int i = 0; i < f_diskrep.files_size; i++) {
      dyn_append(f->files, files_buff[i]);
    }
  }

  return FS_SUCCESS;
}

void file_create(File* f, const char* name, int size, int lba) {
  f->content_size = size;
  f->lba = lba;
  dyn_init(f->name);
  for (int i = 0; i <= strlen(name); i++) {
    dyn_append(f->name, i);
  }
}

void create_file_system() {
  FDR.lba = 1;
  FDR.n_sectors = disk_info.sectors28;
  FDR.next = NULL;
  Folder root;
  folder_create(&root, "root", FDR.lba, 0);
  dyn_append(root.folders, 420);
  dyn_append(root.folders, 69);
  dyn_append(root.files, 42069);
  report_status(folder_to_disk(root));
}

void boot_file_system() {}

void init_file_system() {
  create_file_system();
  // iprintln(FDR.lba, 10);
  Folder f;
  folder_from_disk(1, &f);
  sprintln(f.name.values);
  iprintln(f.folders.values[0], 10);
  iprintln(f.folders.values[1], 10);
  iprintln(f.files.values[0], 10);
  iprintln(f.lba, 10);
  iprintln(f.parent_lba, 10);
  // iprintln(FDR.lba, 10);
}
