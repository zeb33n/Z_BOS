#include "../drivers/disk.h"
#include "../drivers/printing.h"
#include "../utils/memory.h"
#include "../utils/strings.h"
#include "../utils/types.h"
#include "filesystem.h"

void report_status(FileSystemStatus status) {
  switch (status) {
    case FS_ERR_CORRUPT_DISK:
      return sprintln("corrupt disk");
    case FS_SUCCESS:
      return sprintln("ok");
    case FS_ERR_NAME_LENGTH:
      return sprintln("name is too long");
    case FS_ERR_FOLDER_CAP:
      return sprintln("folder at max capacity");
  }
}

// relies on FileTableEntries being 64 byte aligned in disk

// TODO print in different colours

void init_file_system() {}
