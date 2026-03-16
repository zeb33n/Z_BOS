// TODO delete file/folder
// TODO change directory
// TODO rewrite with fileders

#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/data_structures.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

int FDR_LBA;

int current_folder;

void fs_report_status(FileSystemStatus status) {
  switch (status) {
    case FS_ERR_CORRUPT_DISK:
      return sprintln("corrupt disk");
    case FS_ERR_DISK_FULL:
      return sprintln("Disk is full");
    case FS_ERR_NO_NAME:
      return sprintln("corrupt disk");
    case FS_ERR_FILE_NOT_EXIST:
      return sprintln("file does nopt exist");
    case FS_SUCCESS:
      return;
  }
}

#define unwrap_file_status(FS) \
  do {                         \
    if (FS_SUCCESS != FS) {    \
      return FS;               \
    }                          \
  } while (0)

#define unwrap_int(i, FS) \
  do {                    \
    if (i < 0) {          \
      return FS;          \
    }                     \
  } while (0)

void fdr_write(int lba, FreeDiskReigon fdr) {
  FreeDiskReigonUnion fdr_union;
  fdr_union.fdr = fdr;
  write_28bit(MASTER, lba, 1, fdr_union.arr);
}

void fdr_read(int lba, FreeDiskReigon* fdr) {
  FreeDiskReigonUnion fdr_union;
  read_28bit(MASTER, lba, 1, fdr_union.arr);
  *fdr = fdr_union.fdr;
}

int claim_disk_reigon(int n_sectors) {
  FreeDiskReigon fdr;
  fdr_read(FDR_LBA, &fdr);
  int fdr_lba = FDR_LBA;
  for (;;) {
    if (n_sectors <= fdr.n_sectors - fdr.lba_ptr) {
      int out = fdr.lba_ptr;
      fdr.lba_ptr += n_sectors;
      fdr_write(fdr_lba, fdr);
      return out;
    } else if (fdr.next == 0) {
      return -1;
    }
    fdr_lba = fdr.next;
    fdr_read(fdr.next, &fdr);
  }
}

void return_disk_reigon(int lba_free, int n_sectors) {
  if (lba_free <= 0) {
    return;
  }

  FreeDiskReigon fdr = {lba_free, n_sectors, FDR_LBA};
  int lba_fdr = claim_disk_reigon(1);
  fdr_write(lba_fdr, fdr);
  FDR_LBA = lba_fdr;
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

int folder_n_sectors(Folder f) {
  int folder_sectors = (sizeof(int) * f.folders.count + 511) / 512;
  int file_sectors = (sizeof(int) * f.files.count + 511) / 512;
  int name_sectors = (f.name.count + 511) / 512;
  return folder_sectors + file_sectors + name_sectors + 1;
}

void folder_to_disk(int lba, Folder f) {
  int folder_sectors = (sizeof(int) * f.folders.count + 511) / 512;
  int file_sectors = (sizeof(int) * f.files.count + 511) / 512;
  int name_sectors = (f.name.count + 511) / 512;

  int lba_folders = lba + 1;
  int lba_files = lba_folders + folder_sectors;
  int lba_name = lba_files + file_sectors;

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

  if (!name_sectors) {
    return FS_ERR_NO_NAME;
  }

  char name_buff[name_sectors * 512];
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

int file_n_sectors(File f) {
  int name_sectors = (f.name.count + 511) / 512;
  return name_sectors + 1;
}

// returns sector immediately after file -1 if error
void file_to_disk(int lba, File f) {
  int name_sectors = (f.name.count + 511) / 512;

  int lba_name = lba + 1;

  FileUnion f_union;
  f_union.file = f;
  write_28bit(MASTER, lba, 1, f_union.arr);

  if (name_sectors) {
    char name_buff[name_sectors * 512];
    memcopy(name_buff, f.name.values, f.name.count);
    write_28bit(MASTER, lba_name, name_sectors, (short*)name_buff);
  }
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

int file_find_lba(const char* name) {
  Folder folder;
  folder_from_disk_alloc(current_folder, &folder);
  for (int i = 0; i < folder.files.count; i++) {
    File file;
    file_from_disk_alloc(folder.files.values[i], &file);
    if (strcmp(name, file.name.values)) {
      file_free(file);
      return folder.files.values[i];
    }
    file_free(file);
  }
  return -1;
}

void fs_list() {
  int lba = current_folder;
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

FileSystemStatus fs_create_file(const char* name) {
  File file;
  Folder folder;

  file_init_alloc(&file, name, 0, 0);
  unwrap_file_status(folder_from_disk_alloc(current_folder, &folder));

  int file_lba = claim_disk_reigon(file_n_sectors(file));
  unwrap_int(file_lba, FS_ERR_DISK_FULL);
  file_to_disk(file_lba, file);
  file_free(file);

  int folder_sectors_before = folder_n_sectors(folder);
  dyn_append(folder.files, file_lba);
  int folder_sectors_after = folder_n_sectors(folder);
  if (folder_sectors_before < folder_sectors_after) {
    return_disk_reigon(current_folder, folder_sectors_before);
    int folder_lba = claim_disk_reigon(folder_sectors_after);
    unwrap_int(folder_lba, FS_ERR_DISK_FULL);
    current_folder = folder_lba;
  }
  folder_to_disk(current_folder, folder);
  folder_free(folder);

  return FS_SUCCESS;
}

// if i write to a file twice it complains disk is full
FileSystemStatus fs_file_write_content(const char* name,
                                       int content_size,
                                       const char* content) {
  int lba_file = file_find_lba(name);
  unwrap_int(lba_file, FS_ERR_FILE_NOT_EXIST);
  File f;
  unwrap_file_status(file_from_disk_alloc(lba_file, &f));

  return_disk_reigon(f.lba, (f.content_size + 511) / 512);
  int content_sectors = (content_size + 511) / 512;
  int lba_content = claim_disk_reigon(content_sectors);

  unwrap_int(lba_content, FS_ERR_DISK_FULL);
  f.lba = lba_content;
  f.content_size = content_size;
  file_to_disk(lba_file, f);
  write_28bit(MASTER, lba_content, content_sectors, (short*)content);
  file_free(f);

  return FS_SUCCESS;
}

FileSystemStatus fs_file_read_content(const char* name, DynStr* buff) {
  int lba_file = file_find_lba(name);
  unwrap_int(lba_file, FS_ERR_FILE_NOT_EXIST);

  File f;
  unwrap_file_status(file_from_disk_alloc(lba_file, &f));

  int n_sectors = (f.content_size + 511) / 512;
  dyn_init((*buff), n_sectors * 512);
  buff->count = f.content_size;
  read_28bit(MASTER, f.lba, n_sectors, (short*)(buff->values));

  file_free(f);

  return FS_SUCCESS;
}

FileSystemStatus create_file_system() {
  FreeDiskReigon fdr;
  fdr.lba_ptr = 2;
  fdr.n_sectors = disk_info.sectors28;
  fdr.next = 0;
  fdr_write(1, fdr);
  FDR_LBA = 1;
  Folder root;
  folder_init_alloc(&root, "root", 0);
  int lba = claim_disk_reigon(folder_n_sectors(root));
  current_folder = lba;
  unwrap_int(lba, FS_ERR_DISK_FULL);
  folder_to_disk(lba, root);
  return FS_SUCCESS;
}

void boot_file_system() {
  // current_folder = 2;
}

void init_file_system() {
  create_file_system();
  boot_file_system();
}
