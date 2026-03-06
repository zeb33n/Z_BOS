#include "../utils/strings.h"

typedef enum {
  FS_ERR_CORRUPT_DISK,
  FS_ERR_DISK_FULL,
  FS_ERR_NO_NAME,
  FS_SUCCESS

} FileSystemStatus;

typedef struct Folder Folder;
typedef struct FreeDiskReigon FreeDiskReigon;

typedef struct FreeDiskReigon {
  int lba;
  int n_sectors;
  FreeDiskReigon* next;
} FreeDiskReigon;

typedef struct {
  int lba;
  int content_size;
  DynStr name;
} File;

typedef struct {
  int lba;
  int content_size;
  int name_size;
} FileDiskRep;

typedef struct {
  int count;
  int capacity;
  int* values;
} DynIntArr;

typedef struct {
  int lba;
  int parent_lba;
  int name_size;
  int folders_size;
  int files_size;
} FolderDiskRep;

typedef struct Folder {
  DynStr name;
  DynIntArr folders;
  DynIntArr files;
  int lba;
  int parent_lba;
} Folder;

typedef union {
  short arr[256];
  FolderDiskRep folder_disk_rep;
} FolderDiskRepUnion;

void init_file_system();
