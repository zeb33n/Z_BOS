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
  char next;
  FileTableEntryKind kind;
  union {
    Folder folder;
    File file;
  } data;
} FileTableEntry;

typedef struct {
  int count;
  FileTableEntry* entrys;
} FileTable;

void write_file_table();
