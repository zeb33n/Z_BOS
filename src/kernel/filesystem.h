// what if fileders

#include "../utils/strings.h"

typedef enum {
  FS_SUCCESS,
  FS_ERR_CORRUPT_DISK,
  FS_ERR_DISK_FULL,
  FS_ERR_NO_NAME,
  FS_ERR_FILE_NOT_EXIST,
} FileSystemStatus;

typedef struct Folder Folder;

typedef struct FreeDiskReigon {
  int lba;
  int lba_ptr;
  int n_sectors;
  int next;
} FreeDiskReigon;

typedef union {
  short arr[256];
  FreeDiskReigon fdr;
} FreeDiskReigonUnion;

typedef struct {
  int lba;
  int content_size;
  DynStr name;
} File;

typedef union {
  short arr[256];
  File file;
} FileUnion;

typedef struct {
  int count;
  int capacity;
  int* values;
} DynIntArr;

typedef struct Folder {
  DynStr name;
  DynIntArr folders;
  DynIntArr files;
  int parent_lba;
} Folder;

typedef union {
  short arr[256];
  Folder folder;
} FolderUnion;

void init_file_system();
FileSystemStatus fs_create_file(const char* name);
void fs_report_status(FileSystemStatus status);
void fs_list();
FileSystemStatus fs_file_write_content(const char* name,
                                       int content_size,
                                       const char* content);
FileSystemStatus fs_file_read_content(const char* name, char* buff);
