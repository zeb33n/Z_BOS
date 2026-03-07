#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/data_structures.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

// TODO need to be able to write and read FDR's from disk
FreeDiskReigon FDR;

int current_folder;

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

void folder_init_alloc(Folder* f, const char* name, int parent_lba) {
  f->parent_lba = parent_lba;
  dyn_init(f->folders);
  dyn_init(f->files);
  dyn_init(f->name);
  for (int i = 0; i <= strlen(name); i++) {
    dyn_append(f->name, name[i]);
  }
}

void folder_free(Folder f) {
  kfree(f.folders.values);
  kfree(f.files.values);
  kfree(f.name.values);
}

// return sector immediately after, -1 if error
int folder_to_disk(Folder f, int lba) {
  int lba_folders = lba + 1;
  int folder_sectors = (sizeof(int) * f.folders.count + 511) / 512;
  int lba_files = lba_folders + folder_sectors;
  int file_sectors = (sizeof(int) * f.files.count + 511) / 512;
  int lba_name = lba_files + file_sectors;
  int name_sectors = (f.name.count + 511) / 512;
  if (lba_name + name_sectors > disk_info.sectors28) {
    report_status(FS_ERR_DISK_FULL);
    return -1;
  }

  FolderUnion f_union;
  f_union.folder = f;
  write_28bit(MASTER, lba, 1, f_union.arr);

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

  return lba_name + name_sectors;
}

FileSystemStatus folder_from_disk_alloc(int lba, Folder* f) {
  FolderUnion f_union;
  read_28bit(MASTER, lba, 1, f_union.arr);

  int lba_folders = lba + 1;
  int folder_sectors = (sizeof(int) * f_union.folder.folders.count + 511) / 512;
  int lba_files = lba_folders + folder_sectors;
  int file_sectors = (sizeof(int) * f_union.folder.files.count + 511) / 512;
  int lba_name = lba_files + file_sectors;
  int name_sectors = (f_union.folder.name.count + 511) / 512;

  char name_buff[name_sectors * 512];
  if (!name_sectors) {
    return FS_ERR_NO_NAME;
  }

  read_28bit(MASTER, lba_name, name_sectors, (short*)name_buff);
  folder_init_alloc(f, name_buff, f_union.folder.parent_lba);

  int folders_buff[(folder_sectors * 512) / sizeof(int)];
  if (folder_sectors) {
    read_28bit(MASTER, lba_folders, folder_sectors, (short*)folders_buff);
    for (int i = 0; i < f_union.folder.folders.count; i++) {
      dyn_append(f->folders, folders_buff[i]);
    }
  }

  int files_buff[(file_sectors * 512) / sizeof(int)];
  if (file_sectors) {
    read_28bit(MASTER, lba_files, file_sectors, (short*)files_buff);
    for (int i = 0; i < f_union.folder.files.count; i++) {
      dyn_append(f->files, files_buff[i]);
    }
  }

  return FS_SUCCESS;
}

void file_init_alloc(File* f, const char* name, int size, int lba) {
  f->content_size = size;
  f->lba = lba;
  dyn_init(f->name);
  for (int i = 0; i <= strlen(name); i++) {
    dyn_append(f->name, name[i]);
  }
}

void file_free(File f) {
  kfree(f.name.values);
}

// returns sector immediately after file -1 if error
int file_to_disk(File f, int lba) {
  int lba_name = lba + 1;
  int name_sectors = (f.name.count + 511) / 512;
  if (lba_name + name_sectors > disk_info.sectors28) {
    report_status(FS_ERR_DISK_FULL);
    return -1;
  }

  FileUnion f_union;
  f_union.file = f;
  write_28bit(MASTER, lba, 1, f_union.arr);

  if (name_sectors) {
    char name_buff[name_sectors * 512];
    memcopy(name_buff, f.name.values, f.name.count);
    write_28bit(MASTER, lba_name, name_sectors, (short*)name_buff);
  }

  return lba_name + name_sectors;
}

FileSystemStatus file_from_disk_alloc(int lba, File* f) {
  FileUnion f_union;
  read_28bit(MASTER, lba, 1, f_union.arr);
  int name_sectors = (f_union.file.name.count + 511) / 512;

  char name_buff[name_sectors * 512];
  if (!f_union.file.name.count) {
    return FS_ERR_NO_NAME;
  }
  read_28bit(MASTER, lba + 1, name_sectors, (short*)name_buff);
  file_init_alloc(f, name_buff, f_union.file.content_size, f_union.file.lba);
  return FS_SUCCESS;
}

void fs_list(int lba) {
  Folder f;
  folder_from_disk_alloc(lba, &f);
  for (int i = 0; i < f.folders.count; i++) {
    Folder fi;
    folder_from_disk_alloc(f.folders.values[i], &fi);
    sprintln(fi.name.values);
    folder_free(fi);
  }
  for (int i = 0; i < f.files.count; i++) {
    File fi;
    file_from_disk_alloc(f.files.values[i], &fi);
    sprintln(fi.name.values);
    file_free(fi);
  }
  folder_free(f);
}

// TODO add file to current folder
FileSystemStatus fs_create_file(const char* name) {
  File f;
  int lba = FDR.lba;
  file_init_alloc(&f, name, 0, lba);
  int lba_next = file_to_disk(f, lba);
  if (lba_next < 0) {
    return FS_ERR_DISK_FULL;
  }
  FDR.lba = lba_next;
  file_free(f);
  return FS_SUCCESS;
}

FileSystemStatus fs_create_folder(const char* name) {
  Folder f;
  int lba = FDR.lba;
  folder_init_alloc(&f, name, current_folder);
  int lba_next = folder_to_disk(f, lba);
  if (lba_next < 0) {
    return FS_ERR_DISK_FULL;
  }
  FDR.lba = lba_next;
  folder_free(f);
  return FS_SUCCESS;
}

void create_file_system() {
  FDR.lba = 1;
  FDR.n_sectors = disk_info.sectors28;
  FDR.next = NULL;
  Folder root;
  folder_init_alloc(&root, "root", 0);
  folder_to_disk(root, 1);
}

void boot_file_system() {
  current_folder = 1;
}

void init_file_system() {
  create_file_system();
  Folder f;
  folder_from_disk_alloc(1, &f);
  sprintln(f.name.values);
  // File f;
  // file_create(&f, "zebs_file", 10, 3);
  // sprintln(f.name.values);
  // file_to_disk(f);
  // File g;
  // file_from_disk(&g, 3);
  // iprintln(g.content_size, 10);
  // iprintln(g.lba, 10);
  // sprintln(g.name.values);
}
