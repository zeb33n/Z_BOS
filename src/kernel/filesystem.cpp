// TODO: upgrade this with kmalloc

// use the first sector after boot_sectors to store a fat

typedef struct fte FileTableEntry;

typedef enum {
  FILE,
  FOLDER,
} FileTableEntryKind;

typedef struct {
  int start_sector;
  int start_byte;
  int size;
  char name[16];
} File;

typedef struct {
  int numfiles;
  char name[16];
  FileTableEntry* file_table_entry[16];
} Folder;

typedef struct fte {
  FileTableEntryKind Kind;
  union {
    Folder folder;
    File file;
  } data;
} FileTableEntry;

typedef FileTableEntry* FileTable;
