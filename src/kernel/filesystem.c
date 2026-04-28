// TODO delete file/folder
// TODO change directory
// TODO rewrite with fileders

#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/data_structures.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

#define MAX_OPEN_FILEDERS 64

#define NULL_LBA -1

int FDR_LBA;

int CURRENT_FILEDER;

Fileder* OPEN_FILEDERS[MAX_OPEN_FILEDERS];

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
    case FS_ERR_BAD_DESCRIPTOR:
      return sprintlnc("BAD FILEDER DESCRIPTOR", BACKGROUND, RED);
    case FS_ERR_MAX_OPEN_FILES:
      return sprintlnc("MAXIMUM NUMBER OF FILES OPEN", BACKGROUND, RED);
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

void fileder_init_alloc(Fileder* f,
                        const char* name,
                        int parent_lba,
                        int lba,
                        int n_sectors) {
  f->parent_lba = parent_lba;
  f->lba = lba;
  f->n_sectors = n_sectors;
  dyn_init(f->fileders);
  dyn_init(f->name);
  dyn_init(f->content);
  for (int i = 0; i <= strlen(name); i++) {
    dyn_append(f->name, name[i]);
  }
}

void fileder_free(Fileder f) {
  kfree(f.fileders.values);
  kfree(f.name.values);
  kfree(f.content.values);
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

void fileder_to_disk(Fileder f) {
  FilederDiskMap map = fileder_map(f);

  char buff[map.n_sectors * 512];
  memcopy(buff, &f, sizeof(Fileder));
  memcopy(buff + map.name_index, f.name.values, f.name.count);
  memcopy(buff + map.fileders_index, f.fileders.values,
          f.fileders.count * sizeof(int));
  memcopy(buff + map.content_index, f.content.values, f.content.count);

  write_28bit(MASTER, f.lba, map.n_sectors, (short*)buff);
}

void fileder_from_disk_alloc(int lba, Fileder* f) {
  union f_union {
    short arr[256];
    Fileder fileder;
  } f_header;
  read_28bit(MASTER, lba, 1, f_header.arr);

  FilederDiskMap map = fileder_map(f_header.fileder);

  char buff[map.n_sectors * 512];
  read_28bit(MASTER, lba, map.n_sectors, (short*)buff);

  fileder_init_alloc(f, buff + map.name_index, f_header.fileder.parent_lba,
                     f_header.fileder.lba, f_header.fileder.n_sectors);

  int* fileders = (int*)(buff + map.fileders_index);
  dyn_copy_from(f->fileders, f_header.fileder.fileders.count, fileders);

  char* content = buff + map.content_index;
  dyn_copy_from(f->content, f_header.fileder.content.count, content);
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
  Fileder f;
  fileder_from_disk_alloc(CURRENT_FILEDER, &f);
  for (int i = 0; i < f.fileders.count; i++) {
    DynStr dynname;
    fileder_read_name_alloc(f.fileders.values[i], &dynname);
    if (strcmp(name, dynname.values)) {
      fileder_free(f);
      kfree(dynname.values);
      return f.fileders.values[i];
    }
    kfree(dynname.values);
  }
  fileder_free(f);
  return -1;
}

FileSystemStatus fileder_update_lba(int lba, Fileder* f) {
  Fileder* p = OPEN_FILEDERS[CURRENT_FILEDER];
  if (p == NULL) {
    return FS_ERR_BAD_DESCRIPTOR;
  }
  for (int i = 0; i < p->fileders.count; i++) {
    if (p->fileders.values[i] == f->lba) {
      p->fileders.values[i] = lba;
      fs_fileder_save(CURRENT_FILEDER);
      return FS_SUCCESS;
    }
  }
  return FS_ERR_FILE_NOT_EXIST;
}

// need some kind of lock so we cant open the same file twice
int fs_fileder_open(int lba) {
  for (int i = 0; i < MAX_OPEN_FILEDERS; i++) {
    if (OPEN_FILEDERS[i] == NULL) {
      fileder_from_disk_alloc(lba, OPEN_FILEDERS[i]);
      return i;
    }
  }
  return -1;
}

FileSystemStatus fs_fileder_close(int descriptor) {
  Fileder* f = OPEN_FILEDERS[descriptor];
  if (f == NULL) {
    return FS_ERR_BAD_DESCRIPTOR;
  }
  FilederDiskMap f_map = fileder_map(*f);
  if (f_map.n_sectors > f->n_sectors) {
    return_disk_reigon(f->lba, f->n_sectors);
    int lba = claim_disk_reigon(f_map.n_sectors);
    unwrap_int(lba, FS_ERR_DISK_FULL);
    FileSystemStatus status = fileder_update_lba(lba, f);
    unwrap_file_status(status);
  }
  fileder_to_disk(*f);
  fileder_free(*f);
  OPEN_FILEDERS[descriptor] = NULL;
  return FS_SUCCESS;
}

FileSystemStatus fs_fileder_save(int descriptor) {
  Fileder* f = OPEN_FILEDERS[descriptor];
  if (f == NULL) {
    return FS_ERR_BAD_DESCRIPTOR;
  }
  FilederDiskMap f_map = fileder_map(*f);
  if (f_map.n_sectors > f->n_sectors) {
    return_disk_reigon(f->lba, f->n_sectors);
    int lba = claim_disk_reigon(f_map.n_sectors);
    unwrap_int(lba, FS_ERR_DISK_FULL);
    FileSystemStatus status = fileder_update_lba(lba, f);
    unwrap_file_status(status);
  }
  fileder_to_disk(*f);
  return FS_SUCCESS;
}

FileSystemStatus fs_fileder_create(const char* name) {
  Fileder* f;
  Fileder* p = OPEN_FILEDERS[CURRENT_FILEDER];
  if (p == NULL) {
    return FS_ERR_BAD_DESCRIPTOR;
  }
  fileder_init_alloc(f, name, OPEN_FILEDERS[CURRENT_FILEDER]->lba, -1, -1);
  FilederDiskMap f_map = fileder_map(*f);
  f->n_sectors = f_map.n_sectors;
  int lba = claim_disk_reigon(f->n_sectors);
  unwrap_int(lba, FS_ERR_DISK_FULL);
  f->lba = lba;
  dyn_append(OPEN_FILEDERS[CURRENT_FILEDER]->fileders, lba);
  fileder_to_disk(*f);
  fileder_free(*f);
  return FS_SUCCESS;
}

// TODO test with list before you do any more coding mate

FileSystemStatus create_file_system() {
  FreeDiskReigon fdr;
  fdr.lba_ptr = 2;
  fdr.n_sectors = disk_info.sectors28;
  fdr.next = 0;
  fdr_write(1, fdr);
  FDR_LBA = 1;
  Fileder root;
  fileder_init_alloc(&root, "root", 0);
  int lba = claim_disk_reigon(fileder_map(root).n_sectors);
  CURRENT_FILEDER = lba;
  unwrap_int(lba, FS_ERR_DISK_FULL);
  fileder_to_disk(lba, root);
  fileder_free(root);

  return FS_SUCCESS;
}

void boot_file_system() {
  CURRENT_FILEDER = 2;
  for (int i = 0; i < MAX_OPEN_FILEDERS; i++) {
    OPEN_FILEDERS[i] = NULL;
  }
}

// TODO Check that first file is called root if not create a filesystem
void init_file_system() {
  create_file_system();
  boot_file_system();
}
