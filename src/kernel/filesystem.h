#define CONTINUE 0xAA
#define STOP 0xAC

typedef enum { FS_ERR_CORRUPT_DISK, FS_SUCCESS } FileSystemStatus;

typedef struct fte FileTableEntry;

typedef enum {
  FILE,
  FOLDER,
} FileTableEntryKind;

typedef struct {
  int start_sector;
  int start_byte;
  int size;
  char name[10];
} File;

typedef struct {
  int numfiles;
  char name[10];
  FileTableEntry* entrys[10];
} Folder;

typedef struct fte {
  unsigned char next;
  FileTableEntryKind kind;
  union {
    Folder folder;
    File file;
  } data;
} FileTableEntry;

typedef struct {
  int count;
  int capacity;
  FileTableEntry* entrys;
} FileTable;

void init_file_system();
