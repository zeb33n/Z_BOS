#include "../utils/strings.h"

typedef enum {
  FS_ERR_CORRUPT_DISK,
  FS_ERR_NAME_LENGTH,
  FS_ERR_FOLDER_CAP,
  FS_SUCCESS
} FileSystemStatus;

typedef struct {
  int start_sector;
  int start_byte;
  int size;
  DynStr name;
} File;

typedef struct {
  int count;
  int capacity;
  File* values;
} DynFileArr;

typedef struct Folder Folder;

typedef struct {
  int count;
  int capacity;
  Folder* values;
} DynFolderArr;

typedef struct Folder {
  DynStr name;
  DynFolderArr folders;
  DynFileArr files;
} Folder;

void init_file_system();
