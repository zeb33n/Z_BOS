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

#define ROOT_LBA 2

int FDR_LBA;

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
}

Fileder* fileder_init_alloc(const char* name,
                            int parent_lba,
                            int lba,
                            int n_sectors) {
  Fileder* f = kmalloc(sizeof(Fileder));
  f->parent_lba = parent_lba;
  f->lba = lba;
  f->n_sectors = n_sectors;
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
      fileder_init_alloc(buff + map.name_index, f_header.fileder.parent_lba,
                         f_header.fileder.lba, f_header.fileder.n_sectors);

  int* fileders = (int*)(buff + map.fileders_index);
  dyn_copy_from(f->fileders, f_header.fileder.fileders.count, fileders);

  char* content = buff + map.content_index;
  dyn_copy_from(f->content, f_header.fileder.content.count, content);

  return f;
}

FileSystemStatus fileder_update_lba(int lba, Fileder* f) {
  Fileder* p = fileder_from_disk_alloc(f->parent_lba);
  for (int i = 0; i < p->fileders.count; i++) {
    if (p->fileders.values[i] == f->lba) {
      p->fileders.values[i] = lba;
      fileder_to_disk(*p);
      fileder_free(p);
      return FS_SUCCESS;
    }
  }
  fileder_free(p);
  return FS_ERR_FILE_NOT_EXIST;
}

// TODO this is totaly borked FIXIT!
int fileder_lba_from_path(const char* path) {
  int lba = ROOT_LBA;
  if (strcmp(path, "") || strcmp(path, "root")) {
    return lba;
  }
  DynStrArr path_parts = *strsplit_alloc(path);
  Fileder* f = fileder_from_disk_alloc(lba);
  int i = 0;
  for (int j = 0; j < f->fileders.count; j++) {
    Fileder* c = fileder_from_disk_alloc(f->fileders.values[j]);
    if (!strcmp(path_parts.values[i].values, c->name.values)) {
      fileder_free(c);
      continue;
    }
    i++;
    j = 0;
    c = fileder_from_disk_alloc(f->fileders.values[j]);
    fileder_free(f);
    f = c;
  }
}

// need some kind of lock so we cant open the same file twice
// TODO i dont think this should take lbas as an argument
// it should take a name instead
// lets do proper paths.
int fs_fileder_open(const char* name) {
  int lba = fileder_lba_from_path(name);
  unwrap_int(lba, FS_ERR_FILE_NOT_EXIST);

  for (int i = 0; i < MAX_OPEN_FILEDERS; i++) {
    if (OPEN_FILEDERS[i] == NULL) {
      OPEN_FILEDERS[i] = fileder_from_disk_alloc(lba);
      return i;
    }
  }
  return -1;
}

FileSystemStatus fs_fileder_close(int descriptor) {
  Fileder* f = OPEN_FILEDERS[descriptor];
  unwrap_null(f, FS_ERR_BAD_DESCRIPTOR);

  FilederDiskMap f_map = fileder_map(*f);
  if (f_map.n_sectors > f->n_sectors) {
    return_disk_reigon(f->lba, f->n_sectors);
    int lba = claim_disk_reigon(f_map.n_sectors);
    unwrap_int(lba, FS_ERR_DISK_FULL);
    FileSystemStatus status = fileder_update_lba(lba, f);
    unwrap_file_status(status);
  }
  fileder_to_disk(*f);
  fileder_free(f);
  OPEN_FILEDERS[descriptor] = NULL;
  return FS_SUCCESS;
}

FileSystemStatus fs_fileder_save(int descriptor) {
  Fileder* f = OPEN_FILEDERS[descriptor];
  unwrap_null(f, FS_ERR_BAD_DESCRIPTOR);

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

FileSystemStatus fs_fileder_create(int descriptor, const char* name) {
  Fileder* p = OPEN_FILEDERS[descriptor];
  unwrap_null(p, FS_ERR_BAD_DESCRIPTOR);

  Fileder* f = fileder_init_alloc(name, p->lba, -1, -1);

  FilederDiskMap f_map = fileder_map(*f);
  f->n_sectors = f_map.n_sectors;
  int lba = claim_disk_reigon(f->n_sectors);
  unwrap_int(lba, FS_ERR_DISK_FULL);
  f->lba = lba;
  dyn_append(p->fileders, lba);
  fileder_to_disk(*f);
  fileder_free(f);
  return FS_SUCCESS;
}

FileSystemStatus fs_get_name_alloc(int descriptor, DynStr* s) {
  Fileder* f = OPEN_FILEDERS[descriptor];
  unwrap_null(f, FS_ERR_BAD_DESCRIPTOR);
  dyn_init((*s), f->name.count);
  dyn_copy_from((*s), f->name.count, f->name.values);
  return FS_SUCCESS;
}

// TODO rewrite this to not use open!
FileSystemStatus fs_fileders_list_alloc(int descriptor, DynStr* out) {
  Fileder* p = OPEN_FILEDERS[descriptor];
  unwrap_null(p, FS_ERR_BAD_DESCRIPTOR);
  dyn_init((*out));
  for (int i = 0; i < p->fileders.count; i++) {
    int descriptor = fs_fileder_open(p->fileders.values[i]);
    unwrap_int(descriptor, FS_ERR_FILE_NOT_EXIST);
    DynStr name;
    unwrap_file_status(fs_get_name_alloc(descriptor, &name));
    for (int j = 0; j < name.count - 1; j++) {
      dyn_append((*out), name.values[j]);
    }
    dyn_append((*out), '\n');
    kfree(name.values);
    fs_fileder_close(descriptor);
  }
  dyn_append((*out), '\0');
  return FS_SUCCESS;
}

FileSystemStatus create_file_system() {
  // initialise fdr
  FreeDiskReigon fdr;
  fdr.lba_ptr = 2;
  fdr.n_sectors = disk_info.sectors28;
  fdr.next = 0;
  fdr_write(1, fdr);
  FDR_LBA = 1;

  // initialise root
  Fileder* root = fileder_init_alloc("root", -1, -1, -1);
  root->n_sectors = fileder_map(*root).n_sectors;
  int lba = claim_disk_reigon(root->n_sectors);
  unwrap_int(lba, FS_ERR_DISK_FULL);
  root->lba = lba;
  fileder_to_disk(*root);
  fileder_free(root);

  return FS_SUCCESS;
}

FileSystemStatus boot_file_system() {
  for (int i = 0; i < MAX_OPEN_FILEDERS; i++) {
    OPEN_FILEDERS[i] = NULL;
  }

  return FS_SUCCESS;
}

// TODO Check that first file is called root if not create a filesystem
FileSystemStatus init_file_system() {
  unwrap_file_status(create_file_system());
  unwrap_file_status(boot_file_system());
  return FS_SUCCESS;
}
