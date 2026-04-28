
#include "../utils/strings.h"

typedef enum {
  FS_SUCCESS,
  FS_ERR_CORRUPT_DISK,
  FS_ERR_DISK_FULL,
  FS_ERR_NO_NAME,
  FS_ERR_FILE_NOT_EXIST,
  FS_ERR_BAD_DESCRIPTOR,
  FS_ERR_MAX_OPEN_FILES,
} FileSystemStatus;

typedef struct {
  int count;
  int capacity;
  int* values;
} DynIntArr;

typedef struct FreeDiskReigon {
  int lba_ptr;
  int n_sectors;
  int next;
} FreeDiskReigon;

typedef union {
  short arr[256];
  FreeDiskReigon fdr;
} FreeDiskReigonUnion;

typedef struct {
  int parent_lba;
  int lba;
  int n_sectors;
  DynStr name;
  DynStr content;
  DynIntArr fileders;
} Fileder;

typedef struct {
  int name_index;
  int fileders_index;
  int content_index;
  int size_on_disk;
  int n_sectors;
} FilederDiskMap;

void init_file_system();
int fs_fileder_open(int lba);
FileSystemStatus fs_fileder_close(int descriptor);
FileSystemStatus fs_fileder_save(int descriptor);
