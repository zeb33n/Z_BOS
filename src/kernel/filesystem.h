
#include "../utils/strings.h"

typedef enum {
  FS_SUCCESS,
  FS_ERR_CORRUPT_DISK,
  FS_ERR_DISK_FULL,
  FS_ERR_NO_NAME,
  FS_ERR_FILE_NOT_EXIST,
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
FileSystemStatus fs_create_fileder(const char* name);
void fs_report_status(FileSystemStatus status);
void fs_list();
FileSystemStatus fs_fileder_write_content(const char* name,
                                          int content_size,
                                          const char* content);
FileSystemStatus fs_fileder_read_alloc(const char* name, DynStr* buff);
FileSystemStatus fs_delete_fileder(const char* name);
FileSystemStatus fs_change_fileder(const char* name);
