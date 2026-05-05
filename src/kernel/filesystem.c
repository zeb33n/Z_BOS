// TODO delete file/folder
// TODO change directory
// TODO rewrite with fileders

#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/data_structures.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

#define META_LBA 41

#define MAX_OPEN_FILEDERS 64

int FDR_LBA;

int CURRENT_FILEDER;

int OPEN_FILEDER_PTR;

int OPEN_FILEDERS[MAX_OPEN_FILEDERS];

void fs_report_status(FileSystemStatus status) {
  switch (status) {
    case FS_ERR_CORRUPT_DISK:
      return sprintlnc("CORRUPT DISK", BACKGROUND, RED);
    case FS_ERR_DISK_FULL:
      return sprintlnc("DISK FULL", BACKGROUND, RED);
    case FS_ERR_NO_NAME:
      return sprintlnc("NO NAME", BACKGROUND, RED);
    case FS_ERR_FILE_NOT_EXIST:
      return sprintlnc("FILEDER DOES NOT EXIST", BACKGROUND, RED);
    case FS_SUCCESS:
      return;
  }
}

#define unwrap_null(ptr, FS) \
  do {                       \
    if (ptr == NULL) {       \
      return FS;             \
    }                        \
  } while (0)

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

  FileSystemMetaUnion meta;
  read_28bit(MASTER, META_LBA, 1, meta.arr);
  meta.meta.fdr_lba = FDR_LBA;
  write_28bit(MASTER, META_LBA, 1, meta.arr);
}

Fileder* fileder_init_alloc(const char* name, int parent_lba) {
  Fileder* f = kmalloc(sizeof(Fileder));
  f->parent_lba = parent_lba;
  dyn_init(f->fileders);
  dyn_init(f->name);
  dyn_init(f->content);
  for (int i = 0; i <= strlen(name); i++) {
    dyn_append(f->name, name[i]);
  }
  return f;
}

void fileder_free(Fileder* f) {
  kfree(f->fileders.values);
  kfree(f->name.values);
  kfree(f->content.values);
  kfree(f);
}

FilederDiskMap fileder_map(Fileder f) {
  FilederDiskMap map;
  map.name_index = sizeof(Fileder);
  map.fileders_index = map.name_index + f.name.count;
  map.content_index = map.fileders_index + f.fileders.count * sizeof(int);
  map.size_on_disk = map.content_index + f.content.count;
  map.n_sectors = (map.size_on_disk + 511) / 512;
  return map;
}

void fileder_to_disk(int lba, Fileder f) {
  FilederDiskMap map = fileder_map(f);

  char buff[map.n_sectors * 512];
  memcopy(buff, &f, sizeof(Fileder));
  memcopy(buff + map.name_index, f.name.values, f.name.count);
  memcopy(buff + map.fileders_index, f.fileders.values,
          f.fileders.count * sizeof(int));
  memcopy(buff + map.content_index, f.content.values, f.content.count);

  write_28bit(MASTER, lba, map.n_sectors, (short*)buff);
}

Fileder* fileder_from_disk_alloc(int lba) {
  union f_union {
    short arr[256];
    Fileder fileder;
  } f_header;
  read_28bit(MASTER, lba, 1, f_header.arr);

  FilederDiskMap map = fileder_map(f_header.fileder);

  char buff[map.n_sectors * 512];
  read_28bit(MASTER, lba, map.n_sectors, (short*)buff);

  Fileder* f =
      fileder_init_alloc(buff + map.name_index, f_header.fileder.parent_lba);

  int* fileders = (int*)(buff + map.fileders_index);
  dyn_copy_from(f->fileders, f_header.fileder.fileders.count, fileders);

  char* content = buff + map.content_index;
  dyn_copy_from(f->content, f_header.fileder.content.count, content);

  return f;
}

void fileder_read_name_alloc(int lba, DynStr* name) {
  union f_union {
    short arr[256];
    Fileder fileder;
  } f_header;
  read_28bit(MASTER, lba, 1, f_header.arr);

  FilederDiskMap map = fileder_map(f_header.fileder);

  // fileders is the first array after name
  int n_sectors = (sizeof(Fileder) + map.fileders_index + 511) / 512;
  char buff[n_sectors * 512];

  read_28bit(MASTER, lba, n_sectors, (short*)buff);
  char* name_ptr = buff + map.name_index;
  dyn_init((*name));
  dyn_copy_from((*name), f_header.fileder.name.count, name_ptr);
}

int fileder_find_lba(const char* name) {
  Fileder* f = fileder_from_disk_alloc(CURRENT_FILEDER);

  for (int i = 0; i < f->fileders.count; i++) {
    DynStr dynname;
    fileder_read_name_alloc(f->fileders.values[i], &dynname);
    if (strcmp(name, dynname.values)) {
      fileder_free(f);
      kfree(dynname.values);
      return f->fileders.values[i];
    }
    kfree(dynname.values);
  }
  fileder_free(f);
  return -1;
}

FileSystemStatus fileder_update_lba(const char* name, int lba) {
  Fileder* f = fileder_from_disk_alloc(CURRENT_FILEDER);

  for (int i = 0; i < f->fileders.count; i++) {
    DynStr dynname;
    fileder_read_name_alloc(f->fileders.values[i], &dynname);
    if (strcmp(name, dynname.values)) {
      f->fileders.values[i] = lba;
      fileder_to_disk(CURRENT_FILEDER, *f);
      fileder_free(f);
      kfree(dynname.values);
      return FS_SUCCESS;
    }
    kfree(dynname.values);
  }
  fileder_free(f);
  return FS_ERR_FILE_NOT_EXIST;
}

FileSystemStatus fileder_remove_member(const char* name) {
  Fileder* f = fileder_from_disk_alloc(CURRENT_FILEDER);

  for (int i = 0; i < f->fileders.count; i++) {
    DynStr dynname;
    fileder_read_name_alloc(f->fileders.values[i], &dynname);
    if (strcmp(name, dynname.values)) {
      dyn_rm(f->fileders, i);
      fileder_to_disk(CURRENT_FILEDER, *f);
      kfree(dynname.values);
      fileder_free(f);
      return FS_SUCCESS;
    }
    kfree(dynname.values);
  }
  fileder_free(f);
  return FS_ERR_FILE_NOT_EXIST;
}

void fs_list() {
  Fileder* f = fileder_from_disk_alloc(CURRENT_FILEDER);
  for (int i = 0; i < f->fileders.count; i++) {
    DynStr name;
    fileder_read_name_alloc(f->fileders.values[i], &name);
    sprintln(name.values);
    kfree(name.values);
  }
  fileder_free(f);
}

FileSystemStatus fs_create_fileder(const char* name) {
  Fileder* child = fileder_init_alloc(name, CURRENT_FILEDER);
  Fileder* parent = fileder_from_disk_alloc(CURRENT_FILEDER);

  int child_lba = claim_disk_reigon(fileder_map(*child).n_sectors);
  unwrap_int(child_lba, FS_ERR_DISK_FULL);
  fileder_to_disk(child_lba, *child);
  fileder_free(child);

  int parent_sectors_before = fileder_map(*parent).n_sectors;
  dyn_append(parent->fileders, child_lba);
  int parent_sectors_after = fileder_map(*parent).n_sectors;
  if (parent_sectors_before < parent_sectors_after) {
    return_disk_reigon(CURRENT_FILEDER, parent_sectors_before);
    int fileder_lba = claim_disk_reigon(parent_sectors_after);
    unwrap_int(fileder_lba, FS_ERR_DISK_FULL);
    CURRENT_FILEDER = fileder_lba;
  }

  fileder_to_disk(CURRENT_FILEDER, *parent);
  fileder_free(parent);

  return FS_SUCCESS;
}

FileSystemStatus fs_fileder_write_content(const char* name,
                                          int content_size,
                                          const char* content) {
  int lba = fileder_find_lba(name);
  unwrap_int(lba, FS_ERR_FILE_NOT_EXIST);
  Fileder* f = fileder_from_disk_alloc(lba);

  int sectors_before = fileder_map(*f).n_sectors;

  dyn_clear(f->content);
  dyn_copy_from(f->content, content_size, (char*)content);

  int sectors_after = fileder_map(*f).n_sectors;

  if (sectors_before < sectors_after) {
    return_disk_reigon(lba, sectors_before);
    int lba = claim_disk_reigon(sectors_after);
    unwrap_int(lba, FS_ERR_DISK_FULL);
    fileder_update_lba(name, lba);
  }

  fileder_to_disk(lba, *f);
  fileder_free(f);

  return FS_SUCCESS;
}

FileSystemStatus fs_fileder_read_alloc(const char* name, DynStr* buff) {
  int lba = fileder_find_lba(name);
  unwrap_int(lba, FS_ERR_FILE_NOT_EXIST);

  Fileder* f = fileder_from_disk_alloc(lba);

  dyn_init((*buff));
  dyn_copy_from((*buff), f->content.count, f->content.values);

  fileder_free(f);

  return FS_SUCCESS;
}

FileSystemStatus fs_delete_fileder(const char* name) {
  int lba = fileder_find_lba(name);
  unwrap_int(lba, FS_ERR_FILE_NOT_EXIST);
  Fileder* f = fileder_from_disk_alloc(lba);

  int n_sectors = fileder_map(*f).n_sectors;

  return_disk_reigon(lba, n_sectors);

  unwrap_file_status(fileder_remove_member(name));

  fileder_free(f);
  return FS_SUCCESS;
}

FileSystemStatus fs_change_fileder(const char* name) {
  int lba = fileder_find_lba(name);
  unwrap_int(lba, FS_ERR_FILE_NOT_EXIST);
  CURRENT_FILEDER = lba;
  return FS_SUCCESS;
}

FileSystemStatus create_file_system() {
  FDR_LBA = 42;
  CURRENT_FILEDER = 43;

  FreeDiskReigon fdr;
  fdr.lba_ptr = CURRENT_FILEDER;
  fdr.n_sectors = disk_info.sectors28;
  fdr.next = 0;
  fdr_write(FDR_LBA, fdr);
  Fileder* root = fileder_init_alloc("root", 0);
  int lba = claim_disk_reigon(fileder_map(*root).n_sectors);
  if (lba != CURRENT_FILEDER) {
    return FS_ERR_CORRUPT_DISK;
  }
  fileder_to_disk(lba, *root);
  fileder_free(root);

  FileSystemMetaUnion meta;
  meta.meta.fdr_lba = FDR_LBA;
  meta.meta.root_lba = CURRENT_FILEDER;
  meta.meta.check = 0xF8;
  write_28bit(MASTER, META_LBA, 1, meta.arr);

  return FS_SUCCESS;
}

FileSystemStatus boot_file_system() {
  FileSystemMetaUnion meta;
  read_28bit(MASTER, META_LBA, 1, meta.arr);
  if (meta.meta.check != 0xF8) {
    return FS_ERR_CORRUPT_DISK;
  }
  CURRENT_FILEDER = meta.meta.root_lba;
  FDR_LBA = meta.meta.fdr_lba;
  return FS_SUCCESS;
}

FileSystemStatus init_file_system() {
  if (boot_file_system() == FS_ERR_CORRUPT_DISK) {
    sprintln("Creating FileSystem");
    unwrap_file_status(create_file_system());
    return boot_file_system();
  }
  return FS_SUCCESS;
}
